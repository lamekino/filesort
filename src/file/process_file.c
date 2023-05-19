#include "file/process_file.h"

#include "file/get_new_filename.h"
#include "file/recurse_directory.h"
#include "file/file_info.h"
#include "util/str_append.h"
#include "util/error_handling.h"
#include "dir/process_directory.h"
#include "dir/init_dir.h"
#include "util/settings.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

static int rename_wrapper(const settings_t *settings,
                          const char *src,
                          const char *dest) {
    if (settings->transform_file == NULL) {
        printf("renaming '%s' -> '%s'\n", src, dest);
        return rename(src, dest);
    }

    return settings->transform_file(src, dest);
}


status_t process_file(const settings_t *settings,
                      const char *filename,
                      const size_t len) {
    struct file_info info;
    struct stat stat_info;
    char *rename_buffer = NULL;
    int rename_gives;
    status_t status;

    if (stat(filename, &stat_info) < 0) {
        CREATE_STATUS_ERR(status,
                "could not stat file '%s'", filename);
        return status;
    }

    /* if using recursion, don't rename the directory but process the files */
    if (recurse_directory(settings, filename, stat_info)) {
        return STATUS_SKIP;
    }

    /* set the known info about a file */
    info = (struct file_info) {
        .user_settings = settings,
        .filename = filename,
        .creation_time = stat_info.st_ctime,
        .extension = strrchr(filename, '.')
    };

    rename_buffer = malloc(len);
    if (rename_buffer == NULL) {
        CREATE_STATUS_ERR(status,
                "could not allocate space for renamed at file '%s'", filename);
        return status;
    }

    get_new_filename(rename_buffer, len, &info);
    rename_gives = rename_wrapper(settings, info.filename, rename_buffer);
    free(rename_buffer);

    if (rename_gives < 0) {
        return STATUS_FAILED;
    }

    return STATUS_NORMAL;
}
