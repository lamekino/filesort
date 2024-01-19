#include "process_as_directories/process_as_directories.h"

#include <stdlib.h>

#include "types/error.h"
#include "types/settings.h"
#include "types/multistack.h"
#include "process_as_directories/rename_files.h"
#include "process_as_directories/collect_filenames.h"

union error
process_as_directories(const struct settings *settings, char **dirs,
        size_t len) {
    struct multistack files = {0};
    union error err = collect_filenames(&files, dirs, len);

    while (!HAS_ERROR(err) && !is_empty(&files)) {
        err = rename_files(settings, &files);
        if (!HAS_ERROR(err) && !is_empty(&files)) {
            err = collect_filenames(&files, dirs, len);
        }
    }

    cleanup_multistack(&files);
    return err;
}
