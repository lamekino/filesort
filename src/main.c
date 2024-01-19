#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "read_args/read_args.h"
#include "process_as_directories/process_as_directories.h"
#include "process_as_files/process_as_files.h"
#include "types/settings.h"
#include "types/error.h"

int main(int argc, char *argv[]) {
    char starting_path[PATH_LEN];

    size_t number_of_files = 0;
    char **file_list = NULL;

    struct settings settings = {0};
    union error err = {0};

    if (argc < 2) {
        fail("%s requires at least one argument\n", argv[0]);
    }

    /* get the path the program was started in */
    if (!getcwd(starting_path, PATH_MAX)) {
        fail("%s requires at least one argument\n", argv[0]);
    }

    err = read_args(&number_of_files, &file_list, &settings, argc, argv);
    switch (err.level) {
    case LEVEL_SUCCESS:
        err = process_as_files(&settings, file_list, number_of_files);
        break;
    case LEVEL_SPECIAL:
        err = process_as_directories(&settings, file_list, number_of_files);
        break;
    default:
        REPORT_MISSING_IMPL(err);
        break;
    }

    free(file_list);
    return report_error(&err);
}
