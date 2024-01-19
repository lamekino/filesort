#ifndef FILE_INFO_H
#define FILE_INFO_H

#include "types/settings.h"
#include <time.h>

struct file_info {
    int duplicates;
    time_t creation_time;
    char *extension;

    char *prefix;
    char *suffix;
    char *appendix;
};

union error
get_info(struct file_info *dest, const struct settings *settings,
        const char *path);

#endif /* FILE_INFO_H */
