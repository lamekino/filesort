#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "read_args/read_args.h"
#include "read_args/handle_flag.h"
#include "types/error.h"
#include "types/settings.h"

#define HAS_FLAG(xs, idx) ((xs)[(idx)][0] == '-')

static union error
append_file_list(char ***list, size_t *len, char *filename) {
    const size_t idx = *len;
    char **resized = realloc(*list, (idx + 1) * sizeof(*list));

    if (resized == NULL) {
        return ERROR_NO_MEM;
    }
    resized[idx] = filename;

    *list = resized;
    *len += 1;

    return SUCCEED_LEVEL;
}

union error
read_args(size_t *number_of_files, char ***file_list, struct settings *settings,
        int argc, char *argv[]) {
    union error err = {0};
    bool special_level = false;
    int adx = 1;

    while (!HAS_ERROR(err) && adx < argc) {
        const bool skip_flags =
            settings->use_flag_terminator || !HAS_FLAG(argv, adx);

        if (!skip_flags) {
            err = handle_flag(&adx, argc, argv, settings);
            special_level |= IS_SPECIAL(err);
            continue;
        }

        err = append_file_list(file_list, number_of_files, argv[adx]);
        adx++;
    }

    return special_level? SPECIAL_LEVEL : err;
}
