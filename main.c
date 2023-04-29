#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

/* TODO: make this a sensible number */
#define FNAME_MIN 32

#include "arguments.h"
#include "process_directory.h"
#include "error_handling.h"
#include "user_settings.h"
#include "usage.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h> /* PATH_MAX */

int main(int argc, char *argv[]) {
    struct user_settings settings = {0};
    char starting_path[PATH_MAX];
    char current_dir[PATH_MAX];
    int idx;
    char **files_to_process = NULL;
    int number_of_files = 0;

    if (argc < 2) {
        fprintf(stderr, "%s requires at least one argument\n", argv[0]);
        usage(stderr, argv[0]);
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

    /* TODO: move the process of opening directories to its own function */
    /* NOTE: using recursion, the last directory will need to be restored,
     * that infomation is lost in calling process_directory() */
    for (idx = 0; idx < number_of_files; idx++) {
        size_t new_filename_len = 0;
        DIR *dir = NULL;
        int chdir_status = 0;

        realpath(files_to_process[idx], current_dir);
        new_filename_len = PATH_MAX - strnlen(current_dir, PATH_MAX);
        dir = opendir(current_dir);
        chdir_status = chdir(current_dir);

        EXIT_WHEN(new_filename_len < FNAME_MIN,
            "path '%s' too long to rename file", current_dir
        );
        EXIT_WHEN(dir == NULL,
            "could not open directory %s", current_dir
        );
        EXIT_WHEN(chdir_status != 0,
            "can't access directory '%s'", current_dir
        );

        process_directory(&settings, dir, new_filename_len);
        closedir(dir);
        /* restore starting path */
        chdir(starting_path);
    }

    free(files_to_process);

    return EXIT_SUCCESS;
}
