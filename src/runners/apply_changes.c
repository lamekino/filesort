#include "runners/apply_changes.h"

#include "util/settings.h"
#include "util/error_handling.h"
#include "util/init_dir.h"
#include "process_directory/process_directory.h"
#include "file/process_file.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* FIXME: */
#define TMP_VALUE_CHANGE_ME (size_t) 128


status_t apply_on_dir(const settings_t settings,
                      const int number_of_files,
                      const char *starting_path,
                      char **file_list) {
    status_t status = STATUS_NORMAL;
    int idx;

    for (idx = 0; !HAS_ERROR(status) && idx < number_of_files; idx++) {
        DIR *dir = NULL;
        int new_filename_len = init_dir(file_list[idx], &dir);
        if (new_filename_len >= FNAME_MIN) {
            status = process_directory(&settings, dir, new_filename_len);
        }
        else {
            CREATE_STATUS_ERR(status,
                    "error in opening directory '%s'", file_list[idx]);
        }
        closedir(dir);
        chdir(starting_path);
    }

    return status;
}

static status_t chdir_parent_of_file(const char *filepath) {
    (void) filepath;
    UNIMPLEMENTED;
}

status_t apply_on_files(const settings_t settings,
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
    UNIMPLEMENTED;
}
