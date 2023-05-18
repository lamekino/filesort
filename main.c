/* TODO: make this a sensible number */
#define FNAME_MIN 32

#include "arguments.h"
#include "process_directory.h"
#include "error_handling.h"
#include "user_settings.h"
#include "init_dir.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

static status_t apply_on_dir(const struct user_settings settings,
                             const int number_of_files,
                             const char *starting_path,
                             char **file_list) {
    status_t status = STATUS_OK;
    int idx;

    for (idx = 0; !HAS_ERROR(status) && idx < number_of_files; idx++) {
        DIR *dir = NULL;
        int new_filename_len = init_dir(file_list[idx], &dir);
        if (new_filename_len >= FNAME_MIN) {
            status = process_directory(&settings, dir, new_filename_len);
        }
        else {
            status = STATUS_ERR("error in opening file <placeholder>");
        }
        closedir(dir);
        chdir(starting_path);
    }

    return status;
}

int main(int argc, char *argv[]) {
    struct user_settings settings = {0};
    char starting_path[PATH_MAX];
    int idx;
    char **files_to_process = NULL;
    int number_of_files = 0;
    status_t outcome = STATUS_OK;

    EXIT_WHEN(argc < 2,
        "%s requires at least one argument\n", argv[0]
    );

    /* get the path the program was started in */
    EXIT_WHEN(!getcwd(starting_path, PATH_MAX),
        "could not get the current directory"
    );

    number_of_files = read_args(&files_to_process, &settings, argc, argv);
    outcome = apply_on_dir(settings, number_of_files, starting_path, files_to_process);
    free(files_to_process);

    EXIT_WHEN(HAS_ERROR(outcome),
        "%s", outcome.description
    );

    return EXIT_SUCCESS;
}
