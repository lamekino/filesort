#include <string.h>
#include <sys/stat.h>

#include "types/error.h"
#include "types/file_info.h"

union error
get_info(struct file_info *dest, const struct settings *settings,
        const char *path) {
    struct stat file_stat;

    if (stat(path, &file_stat) == -1) {
        return FAILED_LEVEL;
    }

    if (S_ISDIR(file_stat.st_mode)) {
        return SPECIAL_LEVEL;
    }

    dest->extension = strrchr(path, '.');
    dest->duplicates = 0;
    dest->creation_time = file_stat.st_ctime;
    dest->prefix = settings->prefix;
    dest->suffix = settings->suffix;
    dest->appendix = settings->appendix;
    return SUCCEED_LEVEL;
}
