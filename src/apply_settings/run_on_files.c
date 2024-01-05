#include "applications.h"

#include "error_handling.h"
#include "settings.h"

#define TMP_VALUE_CHANGE_ME (size_t) 128

static status_t chdir_parent_of_file(const char *filepath) {
    (void) filepath;
    return STATUS_FAILED;
}

status_t run_on_files(const settings_t *settings,
                           const int number_of_files,
                           const char *starting_path,
                           char **file_list) {
#if 0
    status_t status = STATUS_NORMAL;
    int idx;

    for (idx = 0; !HAS_ERROR(status) && idx < number_of_files; idx++) {
        status = chdir_parent_of_file(file_list[idx]);
        if (IS_NORMAL(status)) {
            status = process_file(&settings, file_list[idx], TMP_VALUE_CHANGE_ME);
        }
        chdir(starting_path);
    }

    return status;
#endif
    (void) settings,
    (void) number_of_files;
    (void) starting_path;
    (void) file_list;
    return STATUS_FAILED;
}
