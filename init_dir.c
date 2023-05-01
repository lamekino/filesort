#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "error_handling.h"
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>

/* TODO: make this a sensible number */
#define FNAME_MIN 32

int init_dir(const char *relative_path, DIR **dir_p) {
    size_t new_filename_len = 0;
    char full_path[PATH_MAX];

    realpath(relative_path, full_path);
    EXIT_WHEN(full_path == NULL,
        "could not alloc path '%s'", relative_path
    );

    new_filename_len = PATH_MAX - strnlen(full_path, PATH_MAX);
    *dir_p = opendir(full_path);

    EXIT_WHEN(new_filename_len < FNAME_MIN,
        "path '%s' too long to rename file", full_path
    );
    EXIT_WHEN(*dir_p == NULL,
        "could not open directory '%s'", full_path
    );
    EXIT_WHEN(chdir(full_path) != 0,
        "can't access directory '%s'", full_path
    );

    return new_filename_len;
}

