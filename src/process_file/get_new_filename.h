#ifndef __GET_NEW_FILENAME_H
#define __GET_NEW_FILENAME_H
#include "process_file/file_info.h"
#include "util/settings.h"
#include <stddef.h>

size_t get_new_filename(char *buffer,
                        const size_t max_len,
                        const settings_t *settings,
                        struct file_info *info);
#endif
