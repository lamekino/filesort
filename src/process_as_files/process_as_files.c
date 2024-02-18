#include "process_as_files/process_as_files.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "filename/apply_rename.h"
#include "types/file_info.h"
#include "types/strings.h"
#include "types/error.h"
#include "types/strings.h"
#include "types/hash_table.h"

union error
process_as_files(const struct settings *settings, char **files, size_t len) {
    size_t i;
    char name_buf[NEW_FILENAME_SIZE] = {0};
    hash_t hashtable[HASH_SIZE] = {0};

    for (i = 0; i < len; i++) {
        struct file_info info;
        union error err = get_info(&info, settings, files[i]);

        switch (err.level) {
        case LEVEL_SUCCESS:
            info.duplicates = hash_info(hashtable, HASH_SIZE, &info);
            err = apply_rename(settings, NULL, files[i], hashtable,
                    sizeof(hashtable));
            break;
        default:
            if (report_warning(&err)) continue;
            REPORT_MISSING_IMPL(err);
            return err;
        }

        switch (err.level) {
        case LEVEL_SUCCESS:
            break;
        default:
            if (report_warning(&err)) continue;
            REPORT_MISSING_IMPL(err);
            return err;
        }
    }

    UNIMPLEMENTED;
}
