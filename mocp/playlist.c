/*
 * MOC - music on console
 * Copyright (C) 2004,2005 Damian Pietras <daper@daper.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Author of title building code: Florian Kriener <me@leflo.de>
 *
 * Contributors:
 *  - Florian Kriener <me@leflo.de> - title building code
 *  - Kamil Tarkowski <kamilt@interia.pl> - plist_prev()
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "common.h"
#include "playlist.h"

/* Initial size of the table */
#define	INIT_SIZE	64

/* Initialize the playlist. */
void plist_init (struct plist *plist)
{
	plist->num = 0;
	plist->allocated = INIT_SIZE;
	plist->not_deleted = 0;
	plist->items = (struct plist_item *)xmalloc (sizeof(struct plist_item)
			* INIT_SIZE);
	plist->serial = -1;
	plist->total_time = 0;
	plist->items_with_time = 0;
}

/* Add a file to the list. Return the index of the item. */
int plist_add (struct plist *plist, const char *file_name)
{
    assert (plist != NULL);
    assert (plist->items != NULL);

    if (plist->allocated == plist->num) {
        plist->allocated *= 2;
        plist->items = (struct plist_item *)xrealloc (plist->items,
                sizeof(struct plist_item) * plist->allocated);
    }

    plist->items[plist->num].file = xstrdup (file_name);
    plist->items[plist->num].type = F_SOUND;//file_name ? file_type (file_name) : F_OTHER;
    plist->items[plist->num].deleted = 0;
    plist->items[plist->num].title_file = NULL;
    plist->items[plist->num].title_tags = NULL;
    plist->items[plist->num].tags = NULL;
    plist->items[plist->num].mtime = (file_name ? get_mtime(file_name)
            : (time_t)-1);
    plist->items[plist->num].queue_pos = 0;

    plist->num++;
    plist->not_deleted++;

    return plist->num - 1;
}

/* Copy the tags data from src to dst freeing old fields if necessary. */
void tags_copy (struct file_tags *dst, const struct file_tags *src)
{
    if (dst->title)
        free (dst->title);
    dst->title = xstrdup (src->title);

    if (dst->artist)
        free (dst->artist);
    dst->artist = xstrdup (src->artist);

    if (dst->album)
        free (dst->album);
    dst->album = xstrdup (src->album);

    dst->track = src->track;
    dst->time = src->time;
    dst->filled = src->filled;
}

struct file_tags *tags_dup (const struct file_tags *tags)
{
    struct file_tags *dtags;

    assert (tags != NULL);

    dtags = tags_new();
    tags_copy (dtags, tags);

    return dtags;
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

struct file_tags *tags_new ()
{
    struct file_tags *tags;

    tags = (struct file_tags *)xmalloc (sizeof(struct file_tags));
    tags->title = NULL;
    tags->artist = NULL;
    tags->album = NULL;
    tags->track = -1;
    tags->time = -1;
    tags->filled = 0;

    return tags;
}
