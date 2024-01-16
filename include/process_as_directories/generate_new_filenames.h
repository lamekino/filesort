#ifndef GENERATE_NEW_FILENAME_H
#define GENERATE_NEW_FILENAME_H

#include "process_file/file_info.h"

size_t
generate_new_filename(char *dest, size_t max_len, const struct file_info *info);

#endif /* GENERATE_NEW_FILENAME_H */
