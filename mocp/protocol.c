#include <assert.h>
#include <string.h>

#include "protocol.h"

#define UNIX_PATH_MAX	108
#define SOCKET_NAME	"socket2"

/* Create a socket name, return NULL if the name could not be created. */
char *socket_name ()
{
    char *socket_name = create_file_name (SOCKET_NAME);

    if (strlen(socket_name) > UNIX_PATH_MAX)
        fatal ("Can't create socket name!");

    return socket_name;
}

/* Get an integer value from the socket, return == 0 on error. */
int get_int (int sock, int *i)
{
    ssize_t res;

    res = recv (sock, i, sizeof(int), 0);
//    if (res == -1)
//        log_errno ("recv() failed when getting int", errno);

    return res == ssizeof(int) ? 1 : 0;
}

/* Send an integer value to the socket, return == 0 on error */
int send_int (int sock, int i)
{
    ssize_t res;

    res = send (sock, &i, sizeof(int), 0);
//    if (res == -1)
//        log_errno ("send() failed", errno);

    return res == ssizeof(int) ? 1 : 0;
}


int send_str (int sock, const char *str)
{
    int len;

    len = strlen (str);
    if (!send_int (sock, len))
        return 0;

    if (send (sock, str, len, 0) != len)
        return 0;

    return 1;
}

/* Push an event on the queue if it's not already there. */
void event_push (struct event_queue *q, const int event, void *data)
{
    assert (q != NULL);

    if (!q->head) {
        q->head = (struct event *)xmalloc (sizeof(struct event));
        q->head->next = NULL;
        q->head->type = event;
        q->head->data = data;
        q->tail = q->head;
    }
    else {
        assert (q->head != NULL);
        assert (q->tail != NULL);
        assert (q->tail->next == NULL);

        q->tail->next = (struct event *)xmalloc (
                sizeof(struct event));
        q->tail = q->tail->next;
        q->tail->next = NULL;
        q->tail->type = event;
        q->tail->data = data;
    }
}

/* Get the string from socket, return NULL on error. The memory is malloced. */
char *get_str (int sock)
{
    int len, nread = 0;
    char *str;

    if (!get_int(sock, &len))
        return NULL;

    if (!RANGE(0, len, MAX_SEND_STRING)) {
//        logit ("Bad string length.");
        return NULL;
    }

    str = (char *)xmalloc (sizeof(char) * (len + 1));
    while (nread < len) {
        ssize_t res;

        res = recv (sock, str + nread, len - nread, 0);
        if (res == -1) {
//            log_errno ("recv() failed when getting string", errno);
            free (str);
            return NULL;
        }
        if (res == 0) {
//            logit ("Unexpected EOF when getting string");
            free (str);
            return NULL;
        }
        nread += res;
    }
    str[len] = 0;

    return str;
}

void free_tag_ev_data (struct tag_ev_response *d)
{
    assert (d != NULL);

    free (d->file);
    tags_free (d->tags);
    free (d);
}
