#ifndef __FILE_INFO_H
#define __FILE_INFO_H
#include "util/settings.h"
#include <time.h>

struct file_info {
    int num_duplicates;
    time_t creation_time;
    char *extension;
    const char *filename;
    const settings_t *user_settings;
};

#endif
