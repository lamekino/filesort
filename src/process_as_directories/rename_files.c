#include "process_as_directories/rename_files.h"

#include <string.h>

#include "types/error.h"
#include "types/strings.h"
#include "types/settings.h"
#include "types/hash_table.h"
#include "types/multistack.h"
#include "generate_new_filename/generate_new_filenames.h"

#define HASH_SIZE 2048u

static union error
apply_rename(const struct settings *settings, const char *basename,
        const char *filepath, int *hash_table, size_t hash_len) {
    union error err = {0};
    struct file_info info = {0};
    char name_buf[NEW_FILENAME_SIZE] = {0};
    char renamed[PATH_LEN] = {0};

    fileop op = settings->operation;
    if (op == NULL) {
        op = &default_rename;
    }

    err = get_info(&info, settings, filepath);
    switch (err.level) {
    case LEVEL_SUCCESS:
        break;
    case LEVEL_FAILED:
        return SUCCEED_LEVEL; /* TODO: log skipping file bc stat */
    case LEVEL_SPECIAL:
        return SPECIAL_LEVEL;
    case LEVEL_NO_MEM:
        return ERROR_NO_MEM;
    default:
        REPORT_MISSING_IMPL(err);
        return err;
    }

    info.duplicates = hash_info(hash_table, hash_len, &info);
    info.duplicates--;

    if (!generate_new_filename(name_buf, sizeof(name_buf), &info)) {
        return create_fatal_err("could not generate filename for: %s",
                filepath);
    }

    if (!path_concat(renamed, basename, name_buf)) {
        return ERROR_NO_MEM;
    }

    if (op(filepath, renamed) < 0) {
        return create_fatal_err("could not rename '%s' to '%s'",
                filepath, renamed);
    }

    return SUCCEED_LEVEL;
}


union error
rename_files(const struct settings *settings, struct multistack *contents) {
    struct stack *directory = NULL;
    const char *fname = NULL;

    struct multistack recurse_dirs = {0};

    int hash_table[HASH_SIZE] = {0};
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
