#include "filename/apply_rename.h"
#include "filename/generate_new_filenames.h"

#include "types/error.h"
#include "types/strings.h"
#include "types/settings.h"
#include "types/hash_table.h"
#include "types/multistack.h"

union error
apply_rename(const struct settings *settings, const char *basename,
        const char *filepath, hash_t *hash_table, size_t hash_len) {
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
        return warning_create("failed to get stat info for '%s', skipping",
                filepath);
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
        return fatal_err_create("could not generate filename for: %s",
                filepath);
    }

    if (!path_concat(renamed, basename, name_buf)) {
        return ERROR_NO_MEM;
    }

    if (op(filepath, renamed) < 0) {
        return fatal_err_create("could not rename '%s' to '%s'",
                filepath, renamed);
    }

    return SUCCEED_LEVEL;
}

