#include "process_as_directories/generate_new_filenames.h"
#include "process_file/file_info.h"
#include "str_append.h"

size_t
generate_new_filename(char *dest, size_t max_len,
        const struct file_info *info) {
    size_t offset = 0;

    #define BUFFER_APPEND(...) \
        do { \
            int y = str_append(dest, offset, max_len, __VA_ARGS__); \
            if (y < 0) return 0; \
            offset += y; \
        } while (0);

    /*
     * Do the pre- timestamp parts
     */
    if (info->prefix) {
        BUFFER_APPEND("%s", info->prefix);
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
    if (info->suffix) {
        BUFFER_APPEND("%s", info->suffix);
    }
    if (info->extension) {
        BUFFER_APPEND("%s", info->extension);
    }
    if (info->appendix) {
        BUFFER_APPEND("%s", info->appendix);
    }

    #undef BUFFER_APPEND

    return offset;
}
