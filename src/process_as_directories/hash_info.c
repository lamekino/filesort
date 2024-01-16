#include "process_as_directories/hash_info.h"
#include "types/file_info.h"

/* http://www.cse.yorku.ca/~oz/hash.html */
static unsigned long
djb2(const char *str) {
    unsigned long hash = 5381;
    int c;

    if (str) {
        while ((c = *str++)) {
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        }
    }

    return hash;
}

size_t
hash_info(int *hash_table, size_t nelem, const struct file_info *info) {
    unsigned long hash = 0;

    hash = info->creation_time;
    hash ^= djb2(info->extension);
    hash ^= djb2(info->prefix);
    hash ^= djb2(info->suffix);
    hash ^= djb2(info->appendix);

    hash_table[hash % nelem] += 1;
    return hash_table[hash % nelem];
}
