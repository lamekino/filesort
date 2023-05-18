#ifndef __PROCESS_FILE_H
#define __PROCESS_FILE_H
#include "error_handling.h"
#include "settings.h"
#include <time.h>

typedef struct {
    int num_duplicates;
    time_t creation_time;
    char *extension;
    const char *filename;
    const settings_t *user_settings;
} file_info_t;

status_t process_file(const settings_t *settings,
                      const char *filename,
                      const size_t len);
#endif
