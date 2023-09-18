#include <assert.h>

#include <sys/un.h>
#include <sys/socket.h>

#include <signal.h>
#include <sys/types.h>
#include <errno.h>

#include "mocp/protocol.h"
#include "mocp/common.h"

#include "mocp/client.h"
#include "mocp/playlist.h"

static struct file_info curr_file;
/* Queue for events coming from the server. */
static struct event_queue events;

/* Initial size of the table */
#define	INIT_SIZE	64

int server_connect()
{
	struct sockaddr_un sock_name;
	int sock;

	/* Create a socket.
	 * For reasons why AF_UNIX is the correct constant to use in both
	 * cases, see the commentary the SVN log for commit r2833. */
	if ((sock = socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
		 return -1;

	sock_name.sun_family = AF_UNIX;
	strcpy (sock_name.sun_path, socket_name());

	if (connect(sock, (struct sockaddr *)&sock_name,
				SUN_LEN(&sock_name)) == -1) {
		close (sock);
		return -1;
	}

	return sock;
}

void close_socket(int sock) {
    close(sock);
}

void server_disconnect(int sock) {
    send_int (sock, CMD_DISCONNECT);
}

int ping_server (int sock)
{
    int event;

    send_int(sock, CMD_PING); /* ignore errors - the server could have
                     already closed the connection and sent
                     EV_BUSY */
    get_int(sock, &event);
//    if (!get_int(sock, &event))
//        fatal ("Error when receiving pong response!");
    return event == EV_PONG ? 1 : 0;
}

int startPlay(int srv_sock) {
   return send_int(srv_sock, CMD_PLAY);
}

void file_info_reset (struct file_info *f)
{
    f->file = NULL;
    f->tags = NULL;
    f->title = NULL;
    f->bitrate = -1;
    f->rate = -1;
    f->curr_time = -1;
    f->total_time = -1;
    f->channels = 1;
    f->state = STATE_STOP;
}

struct file_tags *recv_tags (int sock)
{
    struct file_tags *tags = tags_new ();

    if (!(tags->title = get_str(sock))) {
        info ("Error while receiving title");
    }

    if (!(tags->artist = get_str(sock))) {
        info ("Error while receiving artist");
    }

    if (!(tags->album = get_str(sock))) {
        info ("Error while receiving album");
    }

    if (!get_int(sock, &tags->track)) {
        info ("Error while receiving track");
    }

    if (!get_int(sock, &tags->time)) {
        info ("Error while receiving time");
    }
    return tags;
}

struct tag_ev_response *recv_tags_data_from_srv (int sock)
{
    struct tag_ev_response *r;

    r = (struct tag_ev_response *)xmalloc (sizeof(struct tag_ev_response));

    r->file = get_str_from_srv (sock);
    if (!(r->tags = recv_tags(sock)))
        fatal ("Can't receive tags event's data from the server!");

    return r;
}

/* Receive data for the given type of event and return them. Return NULL if
 * there is no data for the event. */
void *get_event_data (int serv_sock, const int type)
{
    switch (type) {
        case EV_PLIST_ADD:
        case EV_QUEUE_ADD:
            info("recv_item_from_srv ()");
            return NULL;
//            return recv_item_from_srv ();
        case EV_PLIST_DEL:
        case EV_QUEUE_DEL:
        case EV_STATUS_MSG:
        case EV_SRV_ERROR:
            info("get_str_from_srv ()");
            return get_str(serv_sock);
//            return get_str_from_srv ();
        case EV_FILE_TAGS:
            info("recv_tags_data_from_srv ()");
//            return NULL;
            return recv_tags_data_from_srv (serv_sock);
        case EV_PLIST_MOVE:
        case EV_QUEUE_MOVE:
            info("recv_move_ev_data_from_srv ()");
            return NULL;
//            return recv_move_ev_data_from_srv ();
    }

    return NULL;
}

void wait_for_data (int srv_sock)
{
    int event;

    do {
        event = get_int_from_srv (srv_sock);
        if (event == EV_EXIT)
            fatal ("The server exited!");
        if (event != EV_DATA)
            event_push (&events, event, get_event_data(srv_sock, event));
    } while (event != EV_DATA);
}

/* Returned memory is malloc()ed. */
char *get_str_from_srv (int srv_sock)
{
    char *str = get_str (srv_sock);

    if (!str)
        fatal ("Can't receive string from the server!");

    return str;
}

/* Get a string value from the server that will arrive after EV_DATA. */
static char *get_data_str (int srv_sock)
{
    wait_for_data (srv_sock);
    return get_str_from_srv (srv_sock);
}

int get_int_from_srv (int srv_sock)
{
   int num;

   get_int(srv_sock, &num);
//   if (!get_int(srv_sock, &num))
//       fatal ("Can't receive value from the server!");

   return num;
}

int get_data_int (int srv_sock)
{
    wait_for_data (srv_sock);
    return get_int_from_srv (srv_sock);
}

int get_curr_time (int srv_sock)
{
    send_int(srv_sock, CMD_GET_CTIME);
    return get_data_int (srv_sock);
}

char *get_curr_file (int srv_sock)
{
    send_int(srv_sock, CMD_GET_SNAME);
    return get_data_str (srv_sock);
}

int send_tags_request (int sock, const char *file, const int tags_sel)
{
    assert (file != NULL);
    assert (tags_sel != 0);

//    if (file_type(file) == F_SOUND) {
        send_int (sock, CMD_GET_FILE_TAGS);
        send_str (sock, file);
        send_int (sock, tags_sel);
//        debug ("Asking for tags for %s", file);

        return 1;
//    }
//    else {
//        debug ("Not sending tags request for URL (%s)", file);
//        return 0;
//    }
}

struct file_tags *get_tags_no_iface (int sock, const char *file,
        const int tags_sel)
{
    struct file_tags *tags = NULL;

//    assert (file_type(file) == F_SOUND);

    send_tags_request (sock, file, tags_sel);

    while (!tags) {
        int type = get_int_from_srv (sock);
        void *data = get_event_data (sock, type);

        if (type == EV_FILE_TAGS) {
            struct tag_ev_response *ev
                = (struct tag_ev_response *)data;

            if (!strcmp(ev->file, file))
                tags = tags_dup (ev->tags);

            free_tag_ev_data (ev);
        }
        else {
            /* We can't handle other events, since this function
             * is to be invoked without the interface. */
//            logit ("Server sent an event which I didn't expect!");
            abort ();
        }
    }

    return tags;
}

int get_state (int srv_sock)
{
    send_int(srv_sock, CMD_GET_STATE);
    return get_data_int (srv_sock);
}

/* Initialise the block marker information. */
void file_info_block_init (struct file_info *f)
{
    f->block_file = NULL;
}

void getCurrentFileInfo(int sock) {
    file_info_reset (&curr_file);
    file_info_block_init (&curr_file);
    curr_file.file = get_curr_file (sock);
    if (curr_file.file != NULL && strlen(curr_file.file) > 0) {
        curr_file.state = get_state (sock);
        curr_file.curr_time = get_curr_time (sock);
        curr_file.tags = get_tags_no_iface (sock,
                            curr_file.file,
                            TAGS_COMMENTS | TAGS_TIME);
    }
}

void stopPlay(int serv_sock) {
    send_int (serv_sock, CMD_STOP);
}

void switchPause (int srv_sock)
{
    info("getCurrentFileInfo");
    getCurrentFileInfo(srv_sock);
    switch (curr_file.state) {
        case STATE_PLAY:
            info("State play");
            send_int(srv_sock, CMD_PAUSE);
            break;
        case STATE_PAUSE:
            info("State unpause");
            send_int(srv_sock,CMD_UNPAUSE);
            break;
        default:
            info("User pressed pause when not playing.");
    }
}

/* Return 1 if an item has 'deleted' flag. */
inline int plist_deleted (const struct plist *plist, const int num)
{
    assert (LIMIT(num, plist->num));

    return plist->items[num].deleted;
}

/* Send the playlist to the server. If clear != 0, clear the server's playlist
 * before sending. */
void send_playlist (int server_sock, struct plist *plist, const int clear)
{
    int i;

    if (clear)
        send_int(server_sock, CMD_LIST_CLEAR);

    for (i = 0; i < plist->num; i++) {
        if (!plist_deleted(plist, i)) {
            send_int(server_sock, CMD_LIST_ADD);
            send_str(server_sock, plist->items[i].file);
        }
    }
}

void tags_free (struct file_tags *tags)
{
    assert (tags != NULL);

    if (tags->title)
        free (tags->title);
    if (tags->artist)
        free (tags->artist);
    if (tags->album)
        free (tags->album);

    free (tags);
}

void plist_free_item_fields (struct plist_item *item)
{
    if (item->file) {
        free (item->file);
        item->file = NULL;
    }
    if (item->title_tags) {
        free (item->title_tags);
        item->title_tags = NULL;
    }
    if (item->title_file) {
        free (item->title_file);
        item->title_file = NULL;
    }
    if (item->tags) {
        tags_free (item->tags);
        item->tags = NULL;
    }
}

/* Clear the list. */
void plist_clear (struct plist *plist)
{
    int i;

    assert (plist != NULL);

    for (i = 0; i < plist->num; i++)
        plist_free_item_fields (&plist->items[i]);

    plist->items = (struct plist_item *)xrealloc (plist->items,
            sizeof(struct plist_item) * INIT_SIZE);
    plist->allocated = INIT_SIZE;
    plist->num = 0;
    plist->not_deleted = 0;
    plist->total_time = 0;
    plist->items_with_time = 0;
}

/* Destroy the list freeing memory; the list can't be used after that. */
void plist_free (struct plist *plist)
{
    assert (plist != NULL);

    plist_clear (plist);
    free (plist->items);
    plist->allocated = 0;
    plist->items = NULL;
}

void interface_cmdline_playit (int server_sock, char *path)
{
    int serial;
    struct plist plist;

    plist_init (&plist);
    plist_add (&plist, path);
    send_int (server_sock, CMD_LOCK);

    send_playlist (server_sock, &plist, 1);

    send_int (server_sock, CMD_GET_SERIAL);
    serial = get_data_int (server_sock);
    send_int (server_sock, CMD_PLIST_SET_SERIAL);
    send_int (server_sock, serial);

    send_int (server_sock, CMD_UNLOCK);

    send_int (server_sock, CMD_PLAY);
    send_str (server_sock, "");

    plist_free (&plist);
}

int getFileState()
{
    return curr_file.state;
}

char* getCurFile() {
    return curr_file.file;
}

int getTime() {
    if (curr_file.tags != NULL) {
        return curr_file.tags->time;
    }
    return -1;
}

bool isStop() {
    return curr_file.state == STATE_UNDEFINED || curr_file.state == STATE_STOP;
}
