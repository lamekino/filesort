#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "arguments.h"
#include "process_directory.h"
#include "error_handling.h"
#include "user_settings.h"
#include "init_dir.h"
#include "usage.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    struct user_settings settings = {0};
    char starting_path[PATH_MAX];
    int idx;
    char **files_to_process = NULL;
    int number_of_files = 0;

    if (argc < 2) {
        fprintf(stderr, "%s requires at least one argument\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* get the path the program was started in */
    EXIT_WHEN(!getcwd(starting_path, PATH_MAX),
        "could not get the current directory"
    );

    number_of_files = read_args(&files_to_process, &settings, argc, argv);

    EXIT_WHEN(number_of_files == 0,
        "no files provided! doing nothing..."
    );

    for (idx = 0; idx < number_of_files; idx++) {
        DIR *dir = NULL;
        int new_filename_len = init_dir(files_to_process[idx], &dir);

        process_directory(&settings, dir, new_filename_len);
        closedir(dir);
        chdir(starting_path);
    }

    free(files_to_process);

    return EXIT_SUCCESS;
}
