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

union error
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
