#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "cli.h"
#include "process_directory.h"
#include "error_handling.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MASK_XMAP(X) \
    X(HAS_NONE, 0) \
    X(HAS_EXTENSION, 1) \
    X(HAS_DUPLICATE, 2) \
    X(HAS_PREFIX, 4)

typedef enum rename_properties {
    #define ENUMERATE(a, b) a = b,
        MASK_XMAP(ENUMERATE)
    #undef ENUMERATE

    /*
     * NOTE: addition is the same as or for powers of two
     * this enables all the flags while keeping the bits after 0
     */
    #define SET_ALL_BITS(a, b) +(b)
        HAS_ALL_PROPERTIES = MASK_XMAP(SET_ALL_BITS)
    #undef SET_ALL_BITS
} properties_t;

static void possible_filename(char *buffer,
                              const properties_t condition,
                              int duplicates,
                              const size_t max_len,
                              const time_t creation_time,
                              const char *extension,
                              const struct Preferences *state) {
        switch ((int) condition) {
        case HAS_EXTENSION: {
            snprintf(buffer, max_len,
                    "%ld%s", creation_time, extension);
        } break;
        case HAS_DUPLICATE: {
            snprintf(buffer, max_len,
                    "%ld.%d", creation_time, duplicates);
        } break;
        case HAS_PREFIX: {
            snprintf(buffer, max_len,
                    "%s%ld", state->prefix, creation_time);
        } break;
        case HAS_EXTENSION | HAS_DUPLICATE: {
            snprintf(buffer, max_len,
                    "%ld.%d%s", creation_time, duplicates, extension);
        } break;
        case HAS_EXTENSION | HAS_PREFIX: {
            snprintf(buffer, max_len,
                    "%s%ld%s", state->prefix, creation_time, extension);
        } break;
        case HAS_ALL_PROPERTIES: {
            snprintf(buffer, max_len,
                    "%s%ld.%d%s", state->prefix, creation_time, duplicates,
                    extension);
        } break;
        default: { /* has no special properties */
            snprintf(buffer, max_len,
                    "%ld", creation_time);
        } break;
        }
}

static void get_new_filename(char *buffer,
                             const size_t max_len,
                             const char *filename,
                             const struct stat *statinfo,
                             const struct Preferences *state) {
    properties_t props = HAS_NONE;
    int num_duplicates = 0;
    int file_exists = 0;

    const time_t creation_time = statinfo->st_ctime;
    const char *extension = strrchr(filename, '.');

    /* TODO: make this not an abritrary looking shift */
    props |= (extension != NULL) << 0;
    props |= (state->prefix != NULL) << 2;

    while (file_exists || num_duplicates == 0) {
        props |= (num_duplicates > 0) << 1;
        possible_filename(buffer, props, num_duplicates, max_len, creation_time,
                extension, state);
        file_exists = access(buffer, F_OK) == 0;
        num_duplicates++;
    }
}

void process_directory(const struct Preferences *state, DIR *dir, size_t max_fname_len) {
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
                state);

        printf("renaming '%s' -> '%s'\n", old_filename, new_filename);

        /*
         * FIXME: memory leak if this fails
         */
        EXIT_WHEN(rename(old_filename, new_filename) != 0,
                "could not rename file: %s", old_filename);
        free(new_filename);
    }
}
