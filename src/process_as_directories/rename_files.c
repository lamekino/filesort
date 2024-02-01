#include "process_as_directories/rename_files.h"

#include <string.h>

#include "types/error.h"
#include "types/strings.h"
#include "types/settings.h"
#include "types/hash_table.h"
#include "types/multistack.h"
#include "filename/apply_rename.h"

union error
rename_files(const struct settings *settings, struct multistack *contents) {
    struct stack *directory = NULL;
    const char *fname = NULL;

    struct multistack recurse_dirs = {0};

    hash_t hash_table[HASH_SIZE] = {0};
    char fullpath[PATH_LEN] = {0};

    while ((directory = pop_name(contents))) {
        while ((fname = pop_member(directory))) {
            const char *dir = directory->name;
            union error err;

            if (!path_concat(fullpath, dir, fname)) {
                return ERROR_NO_MEM;
            }

            err = apply_rename(settings, dir, fullpath, hash_table,
                        sizeof(hash_table));

            switch (err.level) {
            case LEVEL_SPECIAL:
                if (!push_name(&recurse_dirs, fullpath)) {
                    return ERROR_NO_MEM;
                } /* fall through */
            case LEVEL_SUCCESS:
                break;
            case LEVEL_NO_MEM:
                return ERROR_NO_MEM;
            default:
                REPORT_MISSING_IMPL(err);
                return err;
            }
        }

        memset(hash_table, 0, sizeof(hash_table));
    }

    if (settings->use_recursion) {
        memcpy(contents, &recurse_dirs, sizeof(struct multistack));
    }
    return SUCCEED_LEVEL;
}
