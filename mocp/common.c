#include "common.h"

static int im_server = 0; /* Am I the server? */

/* Return path to a file in MOC config directory. NOT THREAD SAFE */
char *create_file_name (const char *file)
{
    int rc;
    static char fname[PATH_MAX];
    char *moc_dir = options_get_str ("MOCDir");

    if (moc_dir[0] == '~')
        rc = snprintf(fname, sizeof(fname), "%s/%s/%s", get_home (),
                      (moc_dir[1] == '/') ? moc_dir + 2 : moc_dir + 1,
                      file);
    else
        rc = snprintf(fname, sizeof(fname), "%s/%s", moc_dir, file);

    if (rc >= ssizeof(fname))
        fatal ("Path too long!");

    return fname;
}

void fatal(const char *message) {
    fprintf(stderr, "FATAL: %s\n", message);
    exit(EXIT_FATAL);
}

void info(const char *message) {
    fprintf(stderr, "%s\n", message);
}

/* Determine and return the path of the user's home directory. */
const char *get_home ()
{
    static const char *home = NULL;
    struct passwd *passwd;

    if (home == NULL) {
        home = xstrdup (getenv ("HOME"));
        if (home == NULL) {
            errno = 0;
            passwd = getpwuid (geteuid ());
            if (passwd)
                home = xstrdup (passwd->pw_dir);
            else
                if (errno != 0) {
                    char *err = xstrerror (errno);
//                    logit ("getpwuid(%d): %s", geteuid (), err);
                    free (err);
                }
        }
    }

    return home;
}

/* Format a vararg list according to 'format' and return it as a
 * malloc()ed string. */
char *format_msg_va (const char *format, va_list va)
{
    int len;
    char *result;
    va_list va_copy;

    va_copy (va_copy, va);
    len = vsnprintf (NULL, 0, format, va_copy) + 1;
    va_end (va_copy);
    result = xmalloc (len);
    vsnprintf (result, len, format, va);

    return result;
}

void *xmalloc (size_t size)
{
    void *p;

#ifndef HAVE_MALLOC
    size = MAX(1, size);
#endif

    if ((p = malloc(size)) == NULL)
        fatal ("Can't allocate memory!");
    return p;
}

void *xcalloc (size_t nmemb, size_t size)
{
    void *p;

    if ((p = calloc(nmemb, size)) == NULL)
        fatal ("Can't allocate memory!");
    return p;
}

void *xrealloc (void *ptr, const size_t size)
{
    void *p;

    p = realloc (ptr, size);
    if (!p && size != 0)
        fatal ("Can't allocate memory!");

    return p;
}

char *xstrdup (const char *s)
{
    char *n;

    if (s && (n = strdup(s)) == NULL)
        fatal ("Can't allocate memory!");

    return s ? n : NULL;
}

/* Return true iff the argument would be a syntactically valid symbol.
 * (Note that the so-called "peculiar indentifiers" are disallowed here.) */
bool is_valid_symbol (const char *candidate)
{
    size_t len;
    bool result;
    const char *first = "+-.0123456789@";
    const char *valid = "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "0123456789"
                        "@?!.+-*/<=>:$%^&_~";

    result = false;
    len = strlen (candidate);
    if (len > 0 && len == strspn (candidate, valid) &&
                   strchr (first, candidate[0]) == NULL)
        result = true;

    return result;
}

#if !HAVE_DECL_STRERROR_R
static pthread_mutex_t xstrerror_mtx = PTHREAD_MUTEX_INITIALIZER;
#endif

#if !HAVE_DECL_STRERROR_R
/* Return error message in malloc() buffer (for strerror(3)). */
char *xstrerror (int errnum)
{
    char *result;

    /* The client is not threaded. */
    if (!im_server)
        return xstrdup (strerror (errnum));

    LOCK (xstrerror_mtx);

    result = xstrdup (strerror (errnum));

    UNLOCK (xstrerror_mtx);

    return result;
}
#endif
