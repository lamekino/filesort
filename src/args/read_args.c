#include "args/read_args.h"

#include "args/arguments.h"
#include "args/handle_flag.h"

#include "apply_changes.h"
#include "error_handling.h"
#include "transform_file.h"
#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


status_t read_args(int *number_of_files,
                   char ***file_list,
                   settings_t *settings,
                   int argc,
                   char *argv[]) {
    status_t status = STATUS_NORMAL;
    int file_count = 0;
    int adx = 1;

    while (adx < argc) {
        char **file_list_resize = NULL;

        if (!settings->use_flag_terminator && argv[adx][0] == '-') {
            int args_parsed = handle_flag(adx, argc, argv, settings);

            if (args_parsed <= 0) {
                CREATE_STATUS_ERR(status,
                        "invalid argument for flag '%s'", argv[adx]);
                return status;
            }
            adx += args_parsed;
            continue;
        }

        file_count++;
        file_list_resize = realloc(*file_list, file_count * sizeof(*file_list));

        if (file_list_resize == NULL) {
            CREATE_STATUS_ERR(status,
                    "could not allocate memory for file list");
            return status;
        }

        /* set the value of the resized list and make the original pointer point
         * to it */
        file_list_resize[file_count - 1] = argv[adx];
        *file_list = file_list_resize;

        adx++;
    }

    *number_of_files = file_count;
    return status;
}
