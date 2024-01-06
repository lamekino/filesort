#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "process_file.h"
#include "process_file/get_new_filename.h"
#include "process_file/recurse_directory.h"
#include "process_file/file_info.h"

#include "error_handling.h"
#include "settings.h"

static int rename_wrapper(const settings_t *settings,
                          const char *src,
                          const char *dest) {
    if (settings->operation == NULL) {
        printf("renaming '%s' -> '%s'\n", src, dest);
        return rename(src, dest);
    }

    return settings->operation(src, dest);
}


status_t process_file(const settings_t *settings,
                      const char *filename,
                      const size_t len) {
    struct file_info info;
    struct stat stat_info;
    char *rename_buffer = NULL;
    int rename_gives;

    if (stat(filename, &stat_info) < 0) {
        return create_status_err("could not stat file '%s'", filename);
    }

    /* if using recursion, don't rename the directory but process the files */
    if (recurse_directory(settings, filename, stat_info)) {
        return STATUS_SKIP;
    }

    /* set the known info about a file */
    info = (struct file_info) {
        .filename = filename,
        .duplicates = 0,
        .creation_time = stat_info.st_ctime,
        .extension = strrchr(filename, '.')
    };

    rename_buffer = malloc(len);
    if (rename_buffer == NULL) {
        return STATUS_NO_MEM;
    }

    get_new_filename(rename_buffer, len, settings, &info);
    if (info.duplicates >= MAX_DUPLICATES) {
        return create_status_err("hit duplicate cap for '%s'", filename);
    }

    rename_gives = rename_wrapper(settings, filename, rename_buffer);
    free(rename_buffer);

    if (rename_gives < 0) {
        return create_status_err("rename failed for filename '%s'", filename);
    }

    return STATUS_NORMAL;
}
