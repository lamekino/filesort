#ifndef __PROCESS_FILE_H
#define __PROCESS_FILE_H
#include "user_settings.h"
#include <time.h>

int process_file(const struct user_settings *settings,
                 const char *filename,
                 const size_t len);

typedef struct {
    int num_duplicates;
    time_t creation_time;
    char *extension;
    const char *filename;
    const struct user_settings *user_settings;
} file_info_t;
#endif
