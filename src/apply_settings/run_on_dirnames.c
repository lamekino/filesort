#include "apply_settings/applications.h"

#include "util/settings.h"
#include "util/error_handling.h"
#include "util/init_dir.h"
#include "process_directory/process_directory.h"
#include "process_file/process_file.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* FIXME: */


status_t run_on_dirnames(const settings_t *settings,
                      const int number_of_files,
                      const char *starting_path,
                      char **file_list) {
    status_t status = STATUS_NORMAL;
    int idx;

    for (idx = 0; !HAS_ERROR(status) && idx < number_of_files; idx++) {
        DIR *dir = NULL;
        int new_filename_len = init_dir(file_list[idx], &dir);
        if (new_filename_len >= FNAME_MIN) {
            status = process_directory(settings, dir, new_filename_len);
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
