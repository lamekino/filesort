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
    /* mask name -> shift level */ \
    X(HAS_NONE, 0) \
    X(HAS_EXTENSION, 1) \
    X(HAS_DUPLICATE, 2) \
    X(HAS_PREFIX, 4) \
    X(HAS_SUFFIX, 8)

enum rename_properties {
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
};

typedef unsigned int rename_mask_t;

typedef struct {
    int num_duplicates;
    time_t creation_time;
    char *extension;
    const char *filename;
    const struct user_settings *user_settings;
} file_info_t;

static void possible_filename(char *buffer,
                              const size_t max_len,
                              const rename_mask_t properties,
                              const file_info_t *info) {
    const struct user_settings *settings = info->user_settings;
    if ((int) properties & HAS_SUFFIX) {
        UNIMPLEMENTED;
    }
    switch ((int) properties) {
    case HAS_EXTENSION: {
        snprintf(buffer, max_len,
                "%ld%s", info->creation_time, info->extension);
    } break;
    case HAS_DUPLICATE: {
        snprintf(buffer, max_len,
                "%ld.%d", info->creation_time, info->num_duplicates);
    } break;
    case HAS_PREFIX: {
        snprintf(buffer, max_len,
                "%s%ld", settings->prefix, info->creation_time);
    } break;
    case HAS_EXTENSION | HAS_DUPLICATE: {
        snprintf(buffer, max_len,
                "%ld.%d%s", info->creation_time, info->num_duplicates,
                info->extension);
    } break;
    case HAS_EXTENSION | HAS_PREFIX: {
        snprintf(buffer, max_len,
                "%s%ld%s", settings->prefix, info->creation_time,
                info->extension);
    } break;
    case HAS_ALL_PROPERTIES: {
        snprintf(buffer, max_len,
                "%s%ld.%d%s", settings->prefix, info->creation_time,
                info->num_duplicates, info->extension);
    } break;
    default: { /* has no special properties */
        snprintf(buffer, max_len,
                "%ld", info->creation_time);
    } break;
    }
}

static void get_new_filename(char *buffer,
                             const size_t max_len,
                             file_info_t *info,
                             const struct stat *stat) {
    const struct user_settings *settings = info->user_settings;
    rename_mask_t properties = HAS_NONE;
    int file_exists = 0;

    info->creation_time = stat->st_ctime;
    info->extension = strrchr(info->filename, '.');

    /* TODO: make this not an abritrary looking shift */
    properties |= (info->extension != NULL) << 0;
    properties |= (settings->prefix != NULL) << 2;
    properties |= (settings->suffix != NULL) << 3;

    while (file_exists || info->num_duplicates == 0) {
        properties |= (info->num_duplicates > 0) << 1;
        possible_filename(buffer, max_len, properties, info);

        file_exists = access(buffer, F_OK) == 0;
        info->num_duplicates += 1;
    }
}

void process_directory(const struct user_settings *state, DIR *dir, size_t max_fname_len) {
    struct dirent *dir_entry = NULL;
    file_info_t info;
    char confirm[64];

    while ((dir_entry = readdir(dir))) {
        char *rename_buffer = NULL;
        struct stat stat_info;
        /* skip hidden files, parent, current */
        if (dir_entry->d_name[0] == '.') {
            continue;
        }

        /* set the known info about a file */
        info = (file_info_t) {
            .user_settings = state,
            .filename = dir_entry->d_name,
        };

        EXIT_WHEN(stat(info.filename, &stat_info) < 0,
           "could not stat file '%s'", info.filename
        );

        rename_buffer = malloc(max_fname_len);
        EXIT_WHEN(rename_buffer == NULL,
           "could not allocate memory for filename."
        );

        get_new_filename(rename_buffer, max_fname_len, &info, &stat_info);


        /*
         * FIXME: memory leak if this fails
         */
        if (!state->dry_run) {
            printf("renaming '%s' -> '%s'\n",
                    info.filename,
                    rename_buffer);
            EXIT_WHEN(
                rename(info.filename, rename_buffer) != 0,
                "could not rename file: %s", info.filename
            );
        }
        else {
            printf("would rename '%s' -> '%s'\n",
                    info.filename,
                    rename_buffer);
        }

        free(rename_buffer);
    }
}
