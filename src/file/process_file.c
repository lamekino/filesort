#include "file/process_file.h"

#include "util/error_handling.h"
#include "dir/process_directory.h"
#include "dir/init_dir.h"
#include "util/settings.h"

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

#define SET_BIT(name, mask, bit) ((mask) |= ((bit) << (IDX_##name)))

typedef unsigned int rename_mask_t;

static int append(char *buffer,
                  const size_t current_len,
                  const size_t max_len,
                  const char *fmt, ...) {
    size_t chars_written;
    va_list ap;
    va_start(ap, fmt);

    /* these new values represent the current position in the buffer and the
     * new max length given from the current amount written to the buffer */
    char *buf_pos = buffer + current_len;
    size_t delta_len = max_len - current_len;

    chars_written = vsnprintf(buf_pos, delta_len, fmt, ap);

    va_end(ap);
    return chars_written;
}

static size_t get_possible_filename(char *buffer,
                                    const size_t max_len,
                                    const rename_mask_t properties,
                                    const file_info_t *info) {
    const settings_t *settings = info->user_settings;
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

    return offset;
}

static size_t get_new_filename(char *buffer,
                               const size_t max_len,
                               file_info_t *info) {
    const settings_t *settings = info->user_settings;
    rename_mask_t properties = 0;
    int file_exists = 0;
    int num_duplicates = 0;
    size_t name_len = 0;

   SET_BIT(HAS_EXTENSION, properties, info->extension != NULL);
   SET_BIT(HAS_PREFIX, properties, settings->prefix != NULL);
   SET_BIT(HAS_SUFFIX, properties, settings->suffix != NULL);

    while (file_exists || num_duplicates == 0) {
        info->num_duplicates = num_duplicates;
        SET_BIT(HAS_DUPLICATE, properties, info->num_duplicates > 0);

        name_len = get_possible_filename(buffer, max_len, properties, info);
        file_exists = access(buffer, F_OK) == 0;
        num_duplicates += 1;
    }

    return name_len;
}

static int rename_wrapper(const settings_t *settings,
                          const char *src,
                          const char *dest) {
    if (settings->transform_file == NULL) {
        printf("renaming '%s' -> '%s'\n", src, dest);
        return rename(src, dest);
    }

    return settings->transform_file(src, dest);
}

static int recurse_directory(const settings_t *settings,
                             const char *filename,
                             const struct stat stat_info) {
    DIR *next_dir = NULL;
    int len = 0;

    /* not a directory -- do nothing */
    if (!(stat_info.st_mode & S_IFDIR)) {
        return 0;
    }

    /* don't use recursion, skip file */
    if (!settings->use_recursion) {
        fprintf(stderr, "skipping directory '%s'\n", filename);
        return 0;
    }

    /* the file is a direcatory and recursion is enabled */
    len = init_dir(filename, &next_dir);

    /* this is indirect recursion! */
    /* TODO: return this status */
    process_directory(settings, next_dir, len);
    closedir(next_dir);

    /* NOTE: S_IFDIR and IS_ISLNK are mutually exclusive, this will always be
     * parent */
    chdir("..");

    return 1;
}

status_t process_file(const settings_t *settings,
                      const char *filename,
                      const size_t len) {
    file_info_t info;
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
    info = (file_info_t) {
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
