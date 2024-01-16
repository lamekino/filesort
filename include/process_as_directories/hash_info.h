#ifndef HASH_INFO_H
#define HASH_INFO_H

#include "process_file/file_info.h"

size_t
hash_info(int *hash_table, size_t nelem, const struct file_info *info);

#endif /* HASH_INFO_H */
