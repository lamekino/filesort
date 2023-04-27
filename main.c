#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

/* TODO: make this a sensible number */
#define FNAME_MIN 32

#include "arguments.h"
#include "process_directory.h"
#include "error_handling.h"
#include "usage.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* strrchr */
#include <limits.h> /* PATH_MAX */
#include <dirent.h> /* readdir, opendir */
#include <time.h>   /* time_t */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    struct user_settings program_state = {0};
    char starting_path[PATH_MAX];
    char current_dir[PATH_MAX];
    int idx, increment_amount = 1;
    char **files_to_process = NULL;
    int number_of_files = 0;


    if (argc < 2) {
        fprintf(stderr, "%s requires at least one argument\n", argv[0]);
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* get the path the program was started in */
    EXIT_WHEN(!getcwd(starting_path, PATH_MAX),
        "could not get the current directory"
    );

    for (idx = 1; idx < argc; idx += increment_amount) {
        if (argv[idx][0] == '-') {
            increment_amount = handle_flag(idx, argc, argv, &program_state);
        }
        else {
            increment_amount = 1;
            files_to_process = realloc(files_to_process,
                    sizeof(char*) * ++number_of_files);
            EXIT_WHEN(files_to_process == NULL,
                "could not resize file list"
            );
            files_to_process[number_of_files - 1] = argv[idx];
        }
    }

    for (idx = 0; idx < number_of_files; idx++) {
        size_t new_filename_len = 0;
        DIR *dir = NULL;

        realpath(files_to_process[idx], current_dir);
        new_filename_len = PATH_MAX - strnlen(current_dir, PATH_MAX);
        dir = opendir(current_dir);

        EXIT_WHEN(new_filename_len < FNAME_MIN,
            "path '%s' too long to rename file", current_dir
        );
        EXIT_WHEN(dir == NULL,
            "could not open directory %s", current_dir
        );
        EXIT_WHEN(chdir(current_dir) != 0,
            "can't access directory '%s'", current_dir
        );

        process_directory(&program_state, dir, new_filename_len);
        closedir(dir);
        /* restore starting path */
        chdir(starting_path);
    }

    free(files_to_process);

    return EXIT_SUCCESS;
}
