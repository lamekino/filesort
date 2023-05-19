#include "read_args/read_args.h"
#include "apply_settings/apply_settings.h"

#include "util/error_handling.h"
#include "util/init_dir.h"
#include "util/settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int report_errors(status_t s) {
    if (!HAS_ERROR(s)) {
        return EXIT_SUCCESS;
    }

    if (IS_FATAL_ERR(s)) {
        WARNING("fatal memory error! you need more RAM!!");
        return EXIT_FAILURE;
    }

    WARNING("%s", s.description);
    free(s.description);
    return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    char starting_path[PATH_MAX + 1];

    int number_of_files = 0;
    char **files_to_process = NULL;

    settings_t settings = {0};
    status_t outcome = STATUS_NORMAL;

    if (argc < 2) {
        WARNING("%s requires at least one argument\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* get the path the program was started in */
    if (!getcwd(starting_path, PATH_MAX)) {
        WARNING("could not get the current directory");
        return EXIT_FAILURE;
    };

    outcome =
        read_args(&number_of_files, &files_to_process, &settings, argc, argv);
    if (IS_NORMAL(outcome)) {
        outcome =
            apply_settings(&settings, number_of_files, starting_path,
                    files_to_process);
    }

    free(files_to_process);
    return report_errors(outcome);
}
