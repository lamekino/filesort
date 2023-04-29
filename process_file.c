#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "error_handling.h"
#include "process_file.h"
#include "user_settings.h"
#include "arguments.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MASK_XMAP(X) \
    /* mask name -> shift level */ \
    X(HAS_EXTENSION, 0) \
    X(HAS_DUPLICATE, 1) \
    X(HAS_PREFIX, 2) \
    X(HAS_SUFFIX, 3)

enum rename_properties {
    #define ENUMERATE(label, shift_level) label = 1 << shift_level,
        MASK_XMAP(ENUMERATE)
    #undef ENUMERATE
};

enum mask_shift_level {
    /* IDX_HAS_EXTENSION = 0, ... */
    #define ENUMERATE(label, index) IDX_##label = index,
        MASK_XMAP(ENUMERATE)
    #undef ENUMERATE
};

#define SET_BIT(pos, mask, bit) ((mask) |= ((bit) << (pos)))

typedef unsigned int rename_mask_t;

static int append(char *buffer,
                  const size_t current_len,
                  const size_t max_len,
                  const char *fmt, ...) {
    size_t new_len;
    va_list ap;
    va_start(ap, fmt);
    new_len = vsnprintf(buffer + current_len, max_len - current_len, fmt, ap);
    va_end(ap);
    return new_len;
}

static void get_possible_filename(char *buffer,
                              const size_t max_len,
                              const rename_mask_t properties,
                              const file_info_t *info) {
    const struct user_settings *settings = info->user_settings;
    size_t offset = 0;
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
                             file_info_t *info) {
    const struct user_settings *settings = info->user_settings;
    rename_mask_t properties = 0;
    int file_exists = 0;

   SET_BIT(IDX_HAS_EXTENSION, properties, info->extension != NULL);
   SET_BIT(IDX_HAS_PREFIX, properties, settings->prefix != NULL);
   SET_BIT(IDX_HAS_SUFFIX, properties, settings->suffix != NULL);

    while (file_exists || info->num_duplicates == 0) {
        SET_BIT(IDX_HAS_DUPLICATE, properties, info->num_duplicates > 0);

        get_possible_filename(buffer, max_len, properties, info);
        file_exists = access(buffer, F_OK) == 0;
        info->num_duplicates += 1;
    }
}

static int rename_wrapper(const struct user_settings *settings,
                          const char *src,
                          const char *dest) {
    if (settings->transform_file == NULL) {
        printf("renaming '%s' -> '%s'\n", src, dest);
        return rename(src, dest);
    }

    return settings->transform_file(src, dest);
}

int process_file(const struct user_settings *settings,
                 const char *filename,
                 const size_t len) {
    file_info_t info;
    char *rename_buffer = NULL;
    int rename_status = 0;
    struct stat stat_info;

    EXIT_WHEN(stat(filename, &stat_info) < 0,
       "could not stat file '%s'", filename
    );

    /* check if file is directory */
    /* TODO: -R, recurse into directory */
    if (stat_info.st_mode & S_IFDIR) {
        fprintf(stderr, "skipping directory '%s'\n", filename);
        return 1;
    }

    /* set the known info about a file */
    info = (file_info_t) {
        .user_settings = settings,
        .filename = filename,
        .creation_time = stat_info.st_ctime,
        .extension = strrchr(filename, '.')
    };

    rename_buffer = malloc(len);
    EXIT_WHEN(rename_buffer == NULL,
       "could not allocate memory for filename."
    );

    get_new_filename(rename_buffer, len, &info);
    rename_status = rename_wrapper(settings, info.filename, rename_buffer);
    free(rename_buffer);

    return rename_status;
}
