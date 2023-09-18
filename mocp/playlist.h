#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Flags for the info decoder function. */
enum tags_select
{
	TAGS_COMMENTS	= 0x01, /* artist, title, etc. */
	TAGS_TIME	= 0x02 /* time of the file. */
};

struct file_tags
{
	char *title;
	char *artist;
	char *album;
	int track;
	int time;
	int filled; /* Which tags are filled: TAGS_COMMENTS, TAGS_TIME. */
};

enum file_type
{
	F_DIR,
	F_SOUND,
	F_URL,
	F_PLAYLIST,
	F_THEME,
	F_OTHER
};

struct plist_item
{
	char *file;
	enum file_type type;	/* type of the file (F_OTHER if not read yet) */
	char *title_file;	/* title based on the file name */
	char *title_tags;	/* title based on the tags */
	struct file_tags *tags;
	short deleted;
	time_t mtime;		/* modification time */
	int queue_pos;		/* position in the queue */
};

struct plist
{
	int num;			/* Number of elements on the list */
	int allocated;		/* Number of allocated elements */
	int not_deleted;	/* Number of non-deleted items */
	struct plist_item *items;
	int serial;		/* Optional serial number of this playlist */
	int total_time;		/* Total time for files on the playlist */
	int items_with_time;	/* Number of items for which the time is set. */
};

/* Is the string a URL? */
inline int is_url (const char *str)
{
    return !strncasecmp (str, "http://", sizeof ("http://") - 1)
        || !strncasecmp (str, "ftp://", sizeof ("ftp://") - 1);
}

void plist_init (struct plist *plist);
int plist_add (struct plist *plist, const char *file_name);
struct file_tags *tags_dup (const struct file_tags *tags);
struct file_tags *tags_new ();

#ifdef __cplusplus
}
#endif

#endif
