#include "file/file_info.h"
#include "util/str_append.h"
#include "util/settings.h"

#include <unistd.h>

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

static size_t get_possible_filename(char *buffer,
                                    const size_t max_len,
                                    const rename_mask_t properties,
                                    const struct file_info *info) {
    const settings_t *settings = info->user_settings;
    size_t offset = 0;
    /*
     * Do the pre- timestamp parts
     */
    if (properties & HAS_PREFIX) {
        offset += str_append(buffer, offset, max_len,
                "%s", settings->prefix);
    }
    /*
     * add the date info
     */
    offset += str_append(buffer, offset, max_len,
            "%ld", info->creation_time);

    /*
     * do the post- timestamp parts
     */
    if (properties & HAS_DUPLICATE) {
        offset += str_append(buffer, offset, max_len,
                ".%d", info->num_duplicates);
    }
    if (properties & HAS_EXTENSION) {
        offset += str_append(buffer, offset, max_len,
                "%s", info->extension);
    }
    if (properties & HAS_SUFFIX) {
        offset += str_append(buffer, offset, max_len,
                "%s", settings->suffix);
    }

    return offset;
}

size_t get_new_filename(char *buffer,
                        const size_t max_len,
                        struct file_info *info) {
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
