#ifndef __GET_NEW_FILENAME_H
#define __GET_NEW_FILENAME_H
#include "process_file/file_info.h"
#include <stddef.h>

size_t get_new_filename(char *buffer,
                        const size_t max_len,
                        struct file_info *info);
#endif
