#ifndef APPLY_RENAME
#define APPLY_RENAME

#include "types/settings.h"
#include "types/hash_table.h"

union error
apply_rename(const struct settings *settings, const char *basename,
        const char *filepath, hash_t *hash_table, size_t hash_len);

#endif /* APPLY_RENAME */
