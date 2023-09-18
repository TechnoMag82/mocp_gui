#include <time.h>
#include <sys/stat.h>

/* Get the modification time of a file. Return (time_t)-1 on error */
time_t get_mtime (const char *file)
{
    struct stat stat_buf;

    if (stat(file, &stat_buf) != -1)
        return stat_buf.st_mtime;

    return (time_t)-1;
}
