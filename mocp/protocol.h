#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "playlist.h"

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

struct event
{
    int type;	/* type of the event (one of EV_*) */
    void *data;	/* optional data associated with the event */
    struct event *next;
};

struct event_queue
{
    struct event *head;
    struct event *tail;
};

/* Used as data field in the event queue for EV_FILE_TAGS. */
struct tag_ev_response
{
    char *file;
    struct file_tags *tags;
};

/* State of the server. */
#define STATE_UNDEFINED 0x00
#define STATE_PLAY	0x01
#define STATE_STOP	0x02
#define STATE_PAUSE	0x03

#define EV_DATA		0x06 /* data in response to a request will arrive */
#define EV_PONG		0x0b /* response for CMD_PING */
#define EV_EXIT		0x0a /* the server is about to exit */

/* Definition of events sent by server to the client. */
#define EV_STATE	0x01 /* server has changed the state */
#define EV_CTIME	0x02 /* current time of the song has changed */
#define EV_SRV_ERROR	0x04 /* an error occurred */
#define EV_BUSY		0x05 /* another client is connected to the server */
#define EV_DATA		0x06 /* data in response to a request will arrive */
#define EV_BITRATE	0x07 /* the bitrate has changed */
#define EV_RATE		0x08 /* the rate has changed */
#define EV_CHANNELS	0x09 /* the number of channels has changed */
#define EV_EXIT		0x0a /* the server is about to exit */
#define EV_PONG		0x0b /* response for CMD_PING */
#define EV_OPTIONS	0x0c /* the options has changed */
#define EV_SEND_PLIST	0x0d /* request for sending the playlist */
#define EV_TAGS		0x0e /* tags for the current file have changed */
#define EV_STATUS_MSG	0x0f /* followed by a status message */
#define EV_MIXER_CHANGE	0x10 /* the mixer channel was changed */
#define EV_FILE_TAGS	0x11 /* tags in a response for tags request */
#define EV_AVG_BITRATE  0x12 /* average bitrate has changed (new song) */
#define EV_AUDIO_START	0x13 /* playing of audio has started */
#define EV_AUDIO_STOP	0x14 /* playing of audio has stopped */

/* Events caused by a client that wants to modify the playlist (see
 * CMD_CLI_PLIST* commands). */
#define EV_PLIST_ADD	0x50 /* add an item, followed by the file name */
#define EV_PLIST_DEL	0x51 /* delete an item, followed by the file name */
#define EV_PLIST_MOVE	0x52 /* move an item, followed by 2 file names */
#define EV_PLIST_CLEAR	0x53 /* clear the playlist */

/* These events, though similar to the four previous are caused by server
 * which takes care of clients' queue synchronization. */
#define EV_QUEUE_ADD	0x54
#define EV_QUEUE_DEL	0x55
#define EV_QUEUE_MOVE	0x56
#define EV_QUEUE_CLEAR	0x57

#define CMD_PLAY	0x00 /* play the first element on the list */
#define CMD_LIST_CLEAR	0x01 /* clear the list */
#define CMD_LIST_ADD	0x02 /* add an item to the list */
#define CMD_STOP	0x04 /* stop playing */
#define CMD_PAUSE	0x05 /* pause */
#define CMD_UNPAUSE	0x06 /* unpause */

#define CMD_GET_STATE	0x13 /* get the state */
#define CMD_GET_CTIME	0x0d /* get the current song time */
#define CMD_GET_SNAME	0x0f /* get the stream file name */
#define CMD_GET_FILE_TAGS	0x2f	/* get tags for the specified file */

#define CMD_PING	0x19 /* request for EV_PONG */
#define CMD_DISCONNECT	0x15 /* disconnect from the server */

#define CMD_LOCK	0x29 /* acquire a lock */
#define CMD_UNLOCK	0x2a /* release the lock */

#define CMD_GET_SERIAL	0x27 /* get an unique serial number */
#define CMD_PLIST_GET_SERIAL	0x2b /* get the serial number of the server's
                    playlist */
#define CMD_PLIST_SET_SERIAL	0x28 /* assign a serial number to the server's
                    playlist */

char *socket_name ();
int get_int (int sock, int *i);
int send_int (int sock, int i);
int send_str (int sock, const char *str);
char *get_str (int sock);
void event_push (struct event_queue *q, const int event, void *data);
void free_tag_ev_data (struct tag_ev_response *d);

#ifdef __cplusplus
}
#endif

#endif
