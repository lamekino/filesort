#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#include "error_handling.h"
#include "operations.h"
#include "settings.h"
#include "process_file/file_info.h"
#include "process_as_directories/multistack.h"
#include "process_as_directories/generate_new_filenames.h"
#include "process_as_directories/hash_info.h"

#define HASH_SIZE 2048u
#define NEW_FILENAME_SIZE 256u
#define PATH_LEN (PATH_MAX + 1)

/*
 * TODO: create filter options
 */
static bool
is_filtered(const char *filename) {
    /* WARN: we are assuming all files in the list are directories */
    return filename[0] == '.';
}

static status_t
collect_filenames(struct multistack *ms,
        char **dir_list, size_t len) {
    size_t i;
    DIR *dp = NULL;

    for (i = 0; i < len; i++) {
        struct dirent *ent = NULL;

        dp = opendir(dir_list[i]);
        if (!dp || !push_name(ms, dir_list[i])) {
            goto FAIL;
        }

        while ((ent = readdir(dp))) {
            if (is_filtered(ent->d_name)) {
                continue;
            }

            if (!push_member(ms, ent->d_name)) {
                goto FAIL;
            }
        }

        closedir(dp);
    }

    return STATUS_NORMAL;
FAIL:
    if (dp) closedir(dp);
    return STATUS_NO_MEM;
}

static int
get_info(struct file_info *dest, const settings_t *settings, const char *path) {
    struct stat file_stat;

    if (stat(path, &file_stat) == 0) {
        dest->extension = strrchr(path, '.');
        dest->duplicates = 0;
        dest->creation_time = file_stat.st_ctime;

        dest->prefix = settings->prefix;
        dest->suffix = settings->suffix;
        dest->appendix = settings->appendix;
        return 0;
    }

    return -1;
}

static size_t
path_concat(char *dest, size_t max_len, const char *dirname,
        const char *fname) {
    return snprintf(dest, max_len, "%s/%s", dirname, fname);
}

static status_t
apply_rename(const settings_t *settings, const char *dirname,
        const char *filename, int *hash_table, size_t hash_len) {
    struct file_info info = {0};
    char buf[256] = {0};

    char orig[PATH_LEN] = {0};
    char renamed[PATH_LEN] = {0};

    fileop_func op = settings->operation;
    if (op == NULL) {
        op = &default_rename;
    }

    if (!path_concat(orig, sizeof(orig), dirname, filename)) {
        return STATUS_NO_MEM;
    }

    if (get_info(&info, settings, orig) < 0) {
        return create_status_err("could not get file info for: %s",
                orig);
    }

    info.duplicates = hash_info(hash_table, HASH_SIZE, &info);
    if (!generate_new_filename(buf, sizeof(buf), &info)) {
        return create_status_err("could not generate filename for: %s",
                orig);
    }

    if (!path_concat(renamed, sizeof(renamed), dirname, buf)) {
        return STATUS_NO_MEM;
    }

    if (op(orig, renamed) < 0) {
        return create_status_err("could not rename '%s' to '%s'",
                orig, renamed);
    }

    return STATUS_NORMAL;
}

static status_t
rename_files(const settings_t *settings, struct multistack *contents) {
    struct stack *dir = NULL;
    const char *filename = NULL;

    while ((dir = pop_name(contents))) {
        int hash_table[HASH_SIZE] = {0};

        while ((filename = pop_member(dir))) {
            status_t status = apply_rename(settings, dir->name, filename,
                    hash_table, sizeof(hash_table));
            if (!IS_NORMAL(status)) {
                return status;
            }
        }
    }

    return STATUS_NORMAL;
}

status_t
process_as_directories(const settings_t *settings,
        char **dirs, size_t len) {
    struct multistack files = {0};
    status_t status = STATUS_NORMAL;

    status = collect_filenames(&files, dirs, len);
    if (IS_NORMAL(status)) {
        status = rename_files(settings, &files);
    }

    cleanup_multistack(&files);
    return status;
}
