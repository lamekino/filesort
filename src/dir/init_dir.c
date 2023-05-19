#include "dir/init_dir.h"

#include "util/error_handling.h"
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>

int init_dir(const char *relative_path, DIR **dir_p) {
    size_t new_filename_len = 0;
    char *full_path = NULL;

    full_path = realpath(relative_path, full_path);
    if (full_path == NULL) return -1;

    *dir_p = opendir(full_path);
    if (*dir_p == NULL) goto FREE_PATH;
    if (chdir(full_path) != 0) goto CLOSE_DIR;

    new_filename_len = PATH_MAX - strnlen(full_path, PATH_MAX);

    free(full_path);
    return new_filename_len;

CLOSE_DIR:
    closedir(*dir_p);
FREE_PATH:
    free(full_path);
    return -1;
}
