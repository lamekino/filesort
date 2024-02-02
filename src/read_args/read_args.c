#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "read_args/read_args.h"
#include "read_args/handle_flag.h"
#include "types/error.h"
#include "types/settings.h"
#include "types/multistack.h"

#define HAS_FLAG(xs, idx) ((xs)[(idx)][0] == '-')

union error
read_args(size_t *number_of_files, char ***file_list, struct settings *settings,
        int argc, char *argv[]) {
    union error err = {0};
    struct stack files = {0};
    bool enable_file_mode = false;
    int adx;

    for (adx = 1; !HAS_ERROR(err) && adx < argc; adx++) {
        const bool skip_flags =
            settings->use_flag_terminator || !HAS_FLAG(argv, adx);

        if (!skip_flags) {
            err = handle_flag(&adx, argc, argv, settings);
            enable_file_mode |= IS_SPECIAL(err);
            continue;
        }

        if (!push_member_addr(&files, argv[adx])) {
            cleanup_stack(&files);
            return ERROR_NO_MEM;
        }
    }


    *file_list = files.members;
    if (!enable_file_mode) return err;
    else return SPECIAL_LEVEL;
}
