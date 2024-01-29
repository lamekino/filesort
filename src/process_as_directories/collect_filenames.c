#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "types/error.h"
#include "types/multistack.h"

static bool
is_filtered(const char *filename) {
    /* WARN: we are assuming all files in the list are directories */
    return filename[0] == '.';
}

static int
collect_directory(struct multistack *ms, DIR *dp) {
    struct dirent *ent = NULL;

    while ((ent = readdir(dp))) {
        if (is_filtered(ent->d_name)) {
            continue;
        }

        if (!push_member(ms, ent->d_name)) {
            closedir(dp);
            return -1;
        }
    }

    return 0;
}

union error
collect_filenames(struct multistack *ms, char **dir_list, size_t len) {
    size_t i;

    for (i = 0; i < len; i++) {
        DIR *dp = NULL;
        char *dirname = dir_list[i];

        if (!push_name(ms, dirname)) {
            return ERROR_NO_MEM;
        }

        dp = opendir(dirname);
        if (!dp) {
            /* TODO: print warning, but continue */
            return create_fatal_err("could not open dir: '%s'", dirname);
        }

        if (collect_directory(ms, dp) < 0) {
            return ERROR_NO_MEM;
        }
        closedir(dp);
    }

    return SUCCEED_LEVEL;
}
