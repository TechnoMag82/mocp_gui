#include <assert.h>

#include "mocp/lists.h"

struct lists_strs {
    int size;          /* Number of strings on the list */
    int capacity;      /* Number of allocated strings */
    char **strs;
};

/* Clear a list to an empty state. */
void lists_strs_clear (lists_t_strs *list)
{
    int ix;

    assert (list);

    for (ix = 0; ix < list->size; ix += 1)
        free ((void *) list->strs[ix]);
    list->size = 0;
}

/* Free all storage associated with a list of strings. */
void lists_strs_free (lists_t_strs *list)
{
    assert (list);

    lists_strs_clear (list);
    free (list->strs);
    free (list);
}
