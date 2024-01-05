#ifndef GET_NEW_FILENAME_H
#define GET_NEW_FILENAME_H
#include "settings.h"
#include <stddef.h>

struct file_info;

size_t get_new_filename(char *buffer,
                        const size_t max_len,
                        const settings_t *settings,
                        struct file_info *info);
#endif /* GET_NEW_FILENAME_H */
