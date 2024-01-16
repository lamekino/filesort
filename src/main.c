#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "read_args/read_args.h"
#include "process_as_directories/process_as_directories.h"
#include "types/settings.h"
#include "types/error.h"

int main(int argc, char *argv[]) {
    char starting_path[PATH_LEN];

    size_t number_of_files = 0;
    char **file_list = NULL;

    struct settings settings = {0};
    union error level = STATUS_NORMAL;

    if (argc < 2) {
        fprintf(stderr, "%s requires at least one argument\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* get the path the program was started in */
    if (!getcwd(starting_path, PATH_MAX)) {
        fprintf(stderr, "could not get the current directory");
        return EXIT_FAILURE;
    };

    level = read_args(&number_of_files, &file_list, &settings, argc, argv);
    if (IS_NORMAL(level)) {
        level = process_as_directories(&settings, file_list, number_of_files);
    }

    free(file_list);
    return report_error(&level);
}
