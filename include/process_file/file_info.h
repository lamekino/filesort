#ifndef FILE_INFO_H
#define FILE_INFO_H
#include <time.h>
#include "settings.h"

struct file_info {
    int duplicates;
    time_t creation_time;
    char *extension;

    char *prefix;
    char *suffix;
    char *appendix;
};

#endif /* FILE_INFO_H */
