#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "process_directory.h"
#include "error_handling.h"
#include "arguments.h"
#include "user_settings.h"
#include "process_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

void process_directory(const struct user_settings *settings,
                       DIR *dir,
                       size_t max_fname_len) {
    struct dirent *dir_entry = NULL;

    while ((dir_entry = readdir(dir))) {
        int status = 0;
        /* skip hidden files, parent, current */
        if (dir_entry->d_name[0] == '.') {
            continue;
        }

        /* TODO: don't use max size for alloc, do something more sensible */
        status = process_file(settings, dir_entry->d_name, max_fname_len);

        EXIT_WHEN(status < 0,
            "failed to rename '%s'", dir_entry->d_name
        );
    }
}
