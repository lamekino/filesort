#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

/* TODO: make this a sensible number */
#define FNAME_MIN 32

#include "cli.h"
#include "process_directory.h"
#include "error_handling.h"

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
    struct Preferences program_state = {0};
    char starting_path[PATH_MAX];
    char current_dir[PATH_MAX];
    int idx, increment_amount = 1;

    if (argc < 2) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* get the path the program was started in */
    EXIT_WHEN(!getcwd(starting_path, PATH_MAX),
        "could not get the current directory"
    );

    for (idx = 1; idx < argc; idx += increment_amount) {
        size_t new_filename_len = 0;
        DIR *dir = NULL;

        if (argv[idx][0] == '-') {
            increment_amount = handle_flag(idx, argc, argv, &program_state);
            continue;
        }
        increment_amount = 1;

        realpath(argv[idx], current_dir);
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

    return EXIT_SUCCESS;
}
