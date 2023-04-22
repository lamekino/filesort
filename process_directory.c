#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "error_handling.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define INSERT(n) (1u << (n))
/*
 * X-macros:
 * https://stackoverflow.com/a/25354018
 */
#define FLAGS_MAP(X) \
    X(HAS_EXTENSION, 1 << 0) \
    X(HAS_DUPLICATES, 1 << 1) \
    X(HAS_PREFIX, 1 << 2)
#define ENUMERATE(a, b) a = b,
/* note that or is the same as summation with powers of 2, so addition will
 * suffice to get the full bitmask */
#define TOTAL(a, b) +(b)

static void get_new_filename(char *buffer,
                             size_t max_len,
                             const char *filename,
                             const struct stat *statinfo,
                             const char *prefix) {
    enum RenameProperties {
        FLAGS_MAP(ENUMERATE)
        HAS_ALL_PROPERTIES = FLAGS_MAP(TOTAL)
    };
    unsigned int properties = 0;

    int num_duplicates = 0;
    int file_exists = 0;

    const time_t creation_time = statinfo->st_ctime;
    const char *extension = strrchr(filename, '.');

    properties |= extension != NULL;
    properties |= (prefix != NULL) << 2;

    while (file_exists || num_duplicates == 0) {
        switch (properties) {
        case HAS_EXTENSION:
            snprintf(buffer, max_len,
                    "%ld%s", creation_time, extension);
            break;
        case HAS_DUPLICATES:
            snprintf(buffer, max_len,
                    "%ld.%d", creation_time, num_duplicates);
            break;
        case HAS_PREFIX:
            snprintf(buffer, max_len,
                    "%s%ld", prefix, creation_time);
            break;
        case HAS_EXTENSION | HAS_DUPLICATES:
            snprintf(buffer, max_len,
                    "%ld.%d%s", creation_time, num_duplicates, extension);
            break;
        case HAS_EXTENSION | HAS_PREFIX:
            snprintf(buffer, max_len,
                    "%s%ld%s", prefix, creation_time, extension);
            break;
        case HAS_ALL_PROPERTIES:
            snprintf(buffer, max_len,
                    "%s%ld.%d%s",
                    prefix, creation_time, num_duplicates, extension);
            break;
        default: /* has no special properties */
            snprintf(buffer, max_len,
                    "%ld", creation_time);
            break;
        }

        file_exists = access(buffer, F_OK) == 0;
        num_duplicates++;
        properties |= HAS_DUPLICATES;
    }
}

void process_directory(const char *prefix, DIR *dir, size_t max_fname_len) {
    struct dirent *dir_entry = NULL;
    char confirm[64];
    while ((dir_entry = readdir(dir))) {
        struct stat statinfo;
        const char *old_filename = dir_entry->d_name;
        char *new_filename = NULL;
        /* skip hidden files, parent, current */
        if (old_filename[0] == '.') {
            continue;
        }

        EXIT_WHEN(stat(old_filename, &statinfo) < 0,
           "could not stat file '%s'", old_filename
        );

        new_filename = malloc(max_fname_len);
        EXIT_WHEN(new_filename == NULL,
           "could not allocate memory for filename."
        );

        get_new_filename(new_filename, max_fname_len, old_filename, &statinfo,
                prefix);

        printf("renaming '%s' -> '%s'\n", old_filename, new_filename);

        /*
         * FIXME: memory leak if this fails
         */
        EXIT_WHEN(rename(old_filename, new_filename) != 0,
                "could not rename file: %s", old_filename);
        free(new_filename);
    }
}
