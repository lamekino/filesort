#include "process_file/get_new_filename.h"

#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "str_append.h"
#include "settings.h"
#include "process_file/file_info.h"

/* FIXME: this is not thread safe... */
#define file_exists(filename) (access(filename, F_OK) != -1)

static size_t get_possible_filename(char *buffer,
                                    const size_t max_len,
                                    const settings_t *settings,
                                    const struct file_info *info) {
    size_t offset = 0;

    #define BUFFER_APPEND(...) \
        offset += str_append(buffer, offset, max_len, __VA_ARGS__)

    /*
     * Do the pre- timestamp parts
     */
    if (settings->prefix) {
        BUFFER_APPEND("%s", settings->prefix);
    }
    /*
     * add the date info
     */
    BUFFER_APPEND("%ld", info->creation_time);

    /*
     * do the post- timestamp parts
     */
    if (info->duplicates) {
        BUFFER_APPEND(".%d", info->duplicates);
    }
    if (settings->suffix) {
        BUFFER_APPEND("%s", settings->suffix);
    }
    if (info->extension) {
        BUFFER_APPEND("%s", info->extension);
    }
    if (settings->appendix) {
        BUFFER_APPEND("%s", settings->appendix);
    }

    #undef BUFFER_APPEND

    return offset;
}

size_t get_new_filename(char *dest,
                        const size_t max_len,
                        const settings_t *settings,
                        struct file_info *info) {
    size_t name_len = 0;
    char buffer[128];

    while (true) {
        name_len =
            get_possible_filename(buffer, sizeof(buffer), settings, info);

        if (!file_exists(buffer)) break;
        if (++info->duplicates >= MAX_DUPLICATES) break;
    }

    strncpy(dest, buffer, max_len);
    return name_len;
}
