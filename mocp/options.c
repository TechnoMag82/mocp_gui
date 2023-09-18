/*
 * MOC - music on console
 * Copyright (C) 2004 - 2006 Damian Pietras <daper@daper.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <assert.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/types.h>
#include <regex.h>

#include "common.h"
#include "options.h"
#include "lists.h"

#define OPTIONS_MAX	181
#define OPTION_NAME_MAX	32

typedef int options_t_check (int, ...);

union option_value
{
    char *str;
    int num;
    bool boolean;
    lists_t_strs *list;
};

struct option
{
    char name[OPTION_NAME_MAX];
    enum option_type type;
    union option_value value;
    int ignore_in_config;
    int set_in_config;
    unsigned int hash;
    options_t_check *check;
    int count;
    void *constraints;
};

static struct option options[OPTIONS_MAX];
static int options_num = 0;

#define CHECK_NONE          check_true, 0


char *options_get_str (const char *name)
{
    int i = find_option (name, OPTION_STR);

    if (i == -1)
        fprintf (stderr, "Tried to get wrong option '%s'!", name);

    return options[i].value.str;
}

/* Return an index to an option in the options hashtable.
 * If there is no such option return -1. */
static int find_option (const char *name, enum option_type type)
{
    unsigned int h = hash (name), i, init_pos = h % OPTIONS_MAX;

    for (i = init_pos; i < OPTIONS_MAX; i += 1) {
        if (options[i].type == OPTION_FREE)
            return -1;

        if (h == options[i].hash && type & options[i].type) {
            if (!strcasecmp (name, options[i].name))
                return i;
        }
    }

    for (i = 0; i < init_pos; i += 1) {
        if (options[i].type == OPTION_FREE)
            return -1;

        if (h == options[i].hash && type & options[i].type) {
            if (!strcasecmp (name, options[i].name))
                return i;
        }
    }

    return -1;
}

/* Returns the str's hash using djb2 algorithm. */
static unsigned int hash (const char * str)
{
    unsigned int hash = 5381;

    while (*str)
        hash = ((hash << 5) + hash) + tolower(*(str++));
    return hash;
}

/* Always pass a value as valid. */
static int check_true (int unused ATTR_UNUSED, ...)
{
    return 1;
}

/* Check that a string length falls within the specified range(s). */
static int check_length (int opt, ...)
{
    int rc, ix, str_len;
    va_list va;

    assert (opt != -1);
    assert (options[opt].count % 2 == 0);
    assert (options[opt].type & (OPTION_STR | OPTION_LIST));

    rc = 0;
    va_start (va, opt);
    str_len = strlen (va_arg (va, char *));
    for (ix = 0; ix < options[opt].count; ix += 2) {
        if (str_len >= ((int *) options[opt].constraints)[ix] &&
            str_len <= ((int *) options[opt].constraints)[ix + 1]) {
            rc = 1;
            break;
        }
    }
    va_end (va);

    return rc;
}

/* Return an index of a free slot in the options hashtable.
 * If there is no such slot return -1. */
static int find_free (unsigned int h)
{
    unsigned int i;

    assert (options_num < OPTIONS_MAX);
    h %= OPTIONS_MAX;

    for (i = h; i < OPTIONS_MAX; i += 1) {
        if (options[i].type == OPTION_FREE)
            return i;
    }

    for (i = 0; i < h; i += 1) {
        if (options[i].type == OPTION_FREE)
            return i;
    }

    return -1;
}

/* Initializes a position on the options table. This is intended to be used at
 * initialization to make a table of valid options and its default values. */
static int init_option (const char *name, enum option_type type)
{
    unsigned int h=hash(name);
    int pos=find_free(h);

    assert (strlen(name) < OPTION_NAME_MAX);
    assert (is_valid_symbol (name));
    assert(pos>=0);

    strcpy (options[pos].name, name);
    options[pos].hash=h;
    options[pos].type = type;
    options[pos].ignore_in_config = 0;
    options[pos].set_in_config = 0;
    options[pos].check = check_true;
    options[pos].count = 0;
    options[pos].constraints = NULL;

    options_num++;
    return pos;
}

void options_free ()
{
    int i, ix;

    for (i = 0; i < options_num; i++) {
        if (options[i].type == OPTION_STR && options[i].value.str) {
            free (options[i].value.str);
            options[i].value.str = NULL;
        }
        else if (options[i].type == OPTION_LIST) {
            lists_strs_free (options[i].value.list);
            options[i].value.list = NULL;
            for (ix = 0; ix < options[i].count; ix += 1)
                free (((char **) options[i].constraints)[ix]);
        }
        else if (options[i].type == OPTION_SYMB)
            options[i].value.str = NULL;
        if (options[i].type & (OPTION_STR | OPTION_SYMB)) {
            if (options[i].check != check_length) {
                for (ix = 0; ix < options[i].count; ix += 1)
                    free (((char **) options[i].constraints)[ix]);
            }
        }
        options[i].check = check_true;
        options[i].count = 0;
        if (options[i].constraints)
            free (options[i].constraints);
        options[i].constraints = NULL;
    }
}

/* Add a string option to the options table. This is intended to be used at
 * initialization to make a table of valid options and their default values. */
static void add_str (const char *name, const char *value, options_t_check *check, const int count, ...)
{
    int ix, pos;
    va_list va;

    pos = init_option (name, OPTION_STR);
    options[pos].value.str = xstrdup (value);
    options[pos].check = check;
    options[pos].count = count;
    if (count > 0) {
        va_start (va, count);
        if (check == check_length) {
            options[pos].constraints = xcalloc (count, sizeof (int));
            for (ix = 0; ix < count; ix += 1)
                ((int *) options[pos].constraints)[ix] = va_arg (va, int);
        } else {
            options[pos].constraints = xcalloc (count, sizeof (char *));
            for (ix = 0; ix < count; ix += 1)
                ((char **) options[pos].constraints)[ix] = xstrdup (va_arg (va, char *));
        }
        va_end (va);
    }
}

/* Make a table of options and its default values. */
void options_init ()
{
    memset (options, 0, sizeof(options));
    add_str  ("MOCDir", "~/.moc", CHECK_NONE);
}
