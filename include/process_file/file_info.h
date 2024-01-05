#ifndef FILE_INFO_H
#define FILE_INFO_H
#include <time.h>
#include "settings.h"

struct file_info {
    int num_duplicates;
    time_t creation_time;
    char *extension;
    const char *filename;
};

#endif /* FILE_INFO_H */
