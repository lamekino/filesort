#ifndef __PROCESS_FILE_H
#define __PROCESS_FILE_H
#include "error_handling.h"
#include "user_settings.h"
#include <time.h>

typedef struct {
    int num_duplicates;
    time_t creation_time;
    char *extension;
    const char *filename;
    const struct user_settings *user_settings;
} file_info_t;

status_t process_file(const struct user_settings *settings,
                      const char *filename,
                      const size_t len);
#endif
