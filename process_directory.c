#include <stdarg.h>
#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "arguments.h"
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
    X(HAS_EXTENSION, 0) \
    X(HAS_DUPLICATE, 1) \
    X(HAS_PREFIX, 2) \
    X(HAS_SUFFIX, 3)

enum rename_properties {
    #define ENUMERATE(a, b) a = 1 << b,
        MASK_XMAP(ENUMERATE)
    #undef ENUMERATE
};

enum mask_shift_level {
    /* IDX_HAS_EXTENSION = 0, ... */
    #define ENUMERATE(a, b) IDX_##a = b,
        MASK_XMAP(ENUMERATE)
    #undef ENUMERATE
};

#define SET_BIT(pos, mask, bit) ((mask) |= ((bit) << (pos)))

typedef unsigned int rename_mask_t;

typedef struct {
    int num_duplicates;
    time_t creation_time;
    char *extension;
    const char *filename;
    const struct user_settings *user_settings;
} file_info_t;

static int append(char *buffer,
                  const size_t current_len,
                  const size_t max_len,
                  const char *fmt,
                  ...) {
    size_t new_len;
    va_list ap;
    va_start(ap, fmt);
    new_len = vsnprintf(buffer + current_len, max_len - current_len, fmt, ap);
    va_end(ap);
    return new_len;
}

static void possible_filename(char *buffer,
                              const size_t max_len,
                              const rename_mask_t properties,
                              const file_info_t *info) {
    size_t offset = 0;
    const struct user_settings *settings = info->user_settings;
    /*
     * Do the pre- timestamp parts
     */
    if (properties & HAS_PREFIX) {
        offset += append(buffer, offset, max_len,
                "%s", settings->prefix);
    }
    /*
     * add the date info
     */
    offset += append(buffer, offset, max_len,
            "%ld", info->creation_time);

    /*
     * do the post- timestamp parts
     */
    if (properties & HAS_DUPLICATE) {
        offset += append(buffer, offset, max_len,
                ".%d", info->num_duplicates);
    }
    if (properties & HAS_EXTENSION) {
        offset += append(buffer, offset, max_len,
                "%s", info->extension);
    }
    if (properties & HAS_SUFFIX) {
        offset += append(buffer, offset, max_len,
                "%s", settings->suffix);
    }
}

static void get_new_filename(char *buffer,
                             const size_t max_len,
                             file_info_t *info,
                             const struct stat *stat) {
    const struct user_settings *settings = info->user_settings;
    rename_mask_t properties = 0;
    int file_exists = 0;

    info->creation_time = stat->st_ctime;
    info->extension = strrchr(info->filename, '.');

   SET_BIT(IDX_HAS_EXTENSION, properties,
       info->extension != NULL);
   SET_BIT(IDX_HAS_PREFIX, properties,
       settings->prefix != NULL);
   SET_BIT(IDX_HAS_SUFFIX, properties,
       settings->suffix != NULL);

    while (file_exists || info->num_duplicates == 0) {
        SET_BIT(IDX_HAS_DUPLICATE, properties,
            info->num_duplicates > 0);
        possible_filename(buffer, max_len, properties, info);

        file_exists = access(buffer, F_OK) == 0;
        info->num_duplicates += 1;
    }
}

static int confirm_rename(int use_confirm,
                          const char *orig_name,
                          const char *new_name) {
    char user_input[64];

    if (!use_confirm) {
        return 1;
    }

    printf("rename '%s' to '%s'? [y/N] ", orig_name, new_name);
    fgets(user_input, 64, stdin);
    if (user_input[0] == 'y' || user_input[0] == 'Y') {
        return 1;
    }

    printf("skipping '%s'...\n", orig_name);
    return 0;
}

void process_directory(const struct user_settings *settings,
                       DIR *dir,
                       size_t max_fname_len) {
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
            .user_settings = settings,
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


        if (!settings->dry_run) {
            if (!confirm_rename(settings->use_confirm, info.filename, rename_buffer))
                continue;

            printf("renaming '%s' -> '%s'\n",
                    info.filename,
                    rename_buffer);
            /*
             * FIXME: memory leak if this fails
             */
            EXIT_WHEN(
                rename(info.filename, rename_buffer) != 0,
                "could not rename file: %s", info.filename
            );
        }
        else {
            /*
             * BUG: duplicates are ignored since no file is created
             */
            printf("would rename '%s' -> '%s'\n",
                    info.filename,
                    rename_buffer);
        }

        free(rename_buffer);
    }
}
