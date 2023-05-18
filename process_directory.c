#include "process_directory.h"

#include "error_handling.h"
#include "arguments.h"
#include "settings.h"
#include "process_file.h"

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

        if (HAS_ERROR(status)) {
            fprintf(stderr, "failed to rename '%s'\n", dir_entry->d_name);
        }
    }

    return STATUS_OK;
}
