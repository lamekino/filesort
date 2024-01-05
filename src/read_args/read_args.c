#include "read_args/read_args.h"

#include "read_args/arguments.h"
#include "read_args/handle_flag.h"

#include "util/error_handling.h"
#include "util/settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HAS_FLAG(xs, idx) ((xs)[(idx)][0] == '-')

static status_t
append_file_list(char ***list, size_t *len, char *filename) {
    const size_t idx = *len;
    char **resized = realloc(*list, (idx + 1) * sizeof(*list));

    if (resized == NULL) {
        return STATUS_NO_MEM;
    }
    resized[idx] = filename;

    *list = resized;
    *len += 1;

    return STATUS_NORMAL;
}



status_t read_args(size_t *number_of_files,
                   char ***file_list,
                   settings_t *settings,
                   int argc,
                   char *argv[]) {
    status_t status = STATUS_NORMAL;
    int adx = 1;

    while (IS_NORMAL(status) && adx < argc) {
        const bool SKIP_FLAGS =
            settings->use_flag_terminator || !HAS_FLAG(argv, adx);

        if (!SKIP_FLAGS) {
            status = handle_flag(&adx, argc, argv, settings);
            continue;
        }

        status = append_file_list(file_list, number_of_files, argv[adx]);
        adx++;
    }

    return status;
}
