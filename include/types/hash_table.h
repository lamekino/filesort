#ifndef HASH_INFO_H
#define HASH_INFO_H

#include "types/file_info.h"

#define HASH_SIZE 2048u

typedef int hash_t;

size_t
hash_info(int *hash_table, size_t nelem, const struct file_info *info);

#endif /* HASH_INFO_H */
