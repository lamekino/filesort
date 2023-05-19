#include "process_directory/process_directory.h"

#include "util/error_handling.h"
#include "util/settings.h"
#include "process_file/process_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

status_t process_directory(const settings_t *settings,
                           DIR *dir,
                           size_t max_fname_len) {
    struct dirent *dir_entry = NULL;
    status_t status;

    while ((dir_entry = readdir(dir))) {
        /* skip hidden files, parent, current */
        if (dir_entry->d_name[0] == '.') {
            continue;
        }

        /* TODO: don't use max size for alloc, do something more sensible */
        status = process_file(settings, dir_entry->d_name, max_fname_len);

        if (HAS_FAILED(status)) {
            fprintf(stderr, "failed to rename '%s'\n", dir_entry->d_name);
        }
        if (HAS_ERROR(status)) {
            break;
        }
    }

    return STATUS_NORMAL;
}
