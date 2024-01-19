#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "ops/ops.h"
#include "types/file_info.h"
#include "types/error.h"
#include "types/settings.h"
#include "types/multistack.h"
#include "process_as_directories/process_as_directories.h"
#include "process_as_directories/generate_new_filenames.h"
#include "process_as_directories/hash_info.h"

#define HASH_SIZE 2048u
#define NEW_FILENAME_SIZE 256u

/*
 * TODO: create filter options
 */
static bool
is_filtered(const char *filename) {
    /* WARN: we are assuming all files in the list are directories */
    return filename[0] == '.';
}

static size_t
path_concat(char *dest, const char *dirname, const char *fname) {
    return snprintf(dest, PATH_LEN, "%s/%s", dirname, fname);
}


static union error
collect_filenames(struct multistack *ms, char **dir_list, size_t len) {
    size_t i;
    DIR *dp = NULL;

    for (i = 0; i < len; i++) {
        struct dirent *ent = NULL;

        if (!push_name(ms, dir_list[i])) {
            return ERROR_NO_MEM;
        }

        /* TODO: print warning, but continue */
        dp = opendir(dir_list[i]);
        if (!dp) {
            return create_fatal_err("could not open dir: '%s'", dir_list[i]);
        }

        while ((ent = readdir(dp))) {
            if (is_filtered(ent->d_name)) {
                continue;
            }

            if (!push_member(ms, ent->d_name)) {
                closedir(dp);
                return ERROR_NO_MEM;
            }
        }

        closedir(dp);
    }

    return SUCCEED_LEVEL;
}

static union error
get_info(struct file_info *dest, const struct settings *settings,
        const char *path) {
    struct stat file_stat;

    if (stat(path, &file_stat) == -1) {
        return FAILED_LEVEL;
    }

    if (S_ISDIR(file_stat.st_mode)) {
        return SPECIAL_LEVEL;
    }

    dest->extension = strrchr(path, '.');
    dest->duplicates = 0;
    dest->creation_time = file_stat.st_ctime;
    dest->prefix = settings->prefix;
    dest->suffix = settings->suffix;
    dest->appendix = settings->appendix;
    return SUCCEED_LEVEL;
}

static union error
apply_rename(const struct settings *settings, const char *basename,
        const char *filepath, int *hash_table, size_t hash_len) {
    union error err = {0};
    struct file_info info = {0};
    char buf[NEW_FILENAME_SIZE] = {0};

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

    if (!generate_new_filename(buf, sizeof(buf), &info)) {
        return create_fatal_err("could not generate filename for: %s",
                filepath);
    }

    if (!path_concat(renamed, basename, buf)) {
        return ERROR_NO_MEM;
    }

    if (op(filepath, renamed) < 0) {
        return create_fatal_err("could not rename '%s' to '%s'",
                filepath, renamed);
    }

    return SUCCEED_LEVEL;
}


static union error
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
                }
                /* fall through */
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

union error
process_as_directories(const struct settings *settings, char **dirs,
        size_t len) {
    struct multistack files = {0};
    union error err = collect_filenames(&files, dirs, len);

    while (!HAS_ERROR(err) && !is_empty(&files)) {
        err = rename_files(settings, &files);
        if (!HAS_ERROR(err) && !is_empty(&files)) {
            err = collect_filenames(&files, dirs, len);
        }
    }

    cleanup_multistack(&files);
    return err;
}
