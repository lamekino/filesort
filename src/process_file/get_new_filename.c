#include "process_file/get_new_filename.h"

#include <stdbool.h>
#include <unistd.h>

#include "error_handling.h"
#include "str_append.h"
#include "settings.h"
#include "process_file/file_info.h"

#define MAX_DUPLICATES 128u

static size_t get_possible_filename(char *buffer,
                                    const size_t max_len,
                                    const settings_t *settings,
                                    const struct file_info *info) {
    size_t offset = 0;

    #define BUFFER_APPEND(...) offset += \
        str_append(buffer, offset, max_len, __VA_ARGS__)

    /*
     * Do the pre- timestamp parts
     */
    if (settings->prefix != NULL) {
        BUFFER_APPEND("%s", settings->prefix);
    }
    /*
     * add the date info
     */
    BUFFER_APPEND("%ld", info->creation_time);

    /*
     * do the post- timestamp parts
     */
    if (info->num_duplicates > 0) {
        BUFFER_APPEND(".%d", info->num_duplicates);
    }

    if (settings->suffix != NULL) {
        BUFFER_APPEND("%s", settings->suffix);
    }

    if (info->extension != NULL) {
        BUFFER_APPEND("%s", info->extension);
    }

    if (settings->appendix != NULL) {
        BUFFER_APPEND("%s", settings->suffix);
    }

    #undef BUFFER_APPEND

    return offset;
}

size_t get_new_filename(char *buffer,
                        const size_t max_len,
                        const settings_t *settings,
                        struct file_info *info) {
    size_t name_len = 0;
    bool file_exists = false;

    while (file_exists) {
        name_len = get_possible_filename(buffer, max_len, settings, info);

        info->num_duplicates++;
        if (info->num_duplicates == MAX_DUPLICATES) {
            ASSERT(0 && "TODO: implement errors here");
        }
    }

    return name_len;
}
