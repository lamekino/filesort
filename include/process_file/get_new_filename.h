#ifndef GET_NEW_FILENAME_H
#define GET_NEW_FILENAME_H

#include <stddef.h>
#include "settings.h"

#define MAX_DUPLICATES 128
struct file_info;

size_t get_new_filename(char *buffer,
                        const size_t max_len,
                        const settings_t *settings,
                        struct file_info *info);

#endif /* GET_NEW_FILENAME_H */
