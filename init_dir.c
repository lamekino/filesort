#include "error_handling.h"
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>

int init_dir(const char *relative_path, DIR **dir_p) {
    size_t new_filename_len = 0;
    char *full_path = NULL;

    full_path = realpath(relative_path, full_path);
    EXIT_WHEN(full_path == NULL,
        "could not alloc path '%s'", relative_path
    );

    *dir_p = opendir(full_path);

    EXIT_WHEN(*dir_p == NULL,
        "could not open directory '%s'", full_path
    );
    EXIT_WHEN(chdir(full_path) != 0,
        "can't access directory '%s'", full_path
    );

    new_filename_len = PATH_MAX - strnlen(full_path, PATH_MAX);

    free(full_path);
    return new_filename_len;
}

