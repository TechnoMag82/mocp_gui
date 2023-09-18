#ifndef OPTIONS_H
#define OPTIONS_H

//#include "lists.h"

#ifdef __cplusplus
extern "C" {
#endif

enum option_type
{
	OPTION_FREE = 0,
	OPTION_INT  = 1,
	OPTION_BOOL = 2,
	OPTION_STR  = 4,
	OPTION_SYMB = 8,
	OPTION_LIST = 16,
	OPTION_ANY  = 255
};

static int find_option (const char *name, enum option_type type);
static unsigned int hash (const char * str);

char *options_get_str (const char *name);
void options_init ();
void options_free ();

enum option_type options_get_type (const char *name);

#ifdef __cplusplus
}
#endif

#endif
