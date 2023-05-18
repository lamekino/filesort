#include "arguments.h"
#include "process_directory.h"
#include "error_handling.h"
#include "settings.h"
#include "init_dir.h"
#include "apply_changes.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    char starting_path[PATH_MAX + 1];

    int number_of_files = 0;
    char **files_to_process = NULL;

    settings_t settings = { .execute = &apply_on_dir };
    status_t outcome = STATUS_NORMAL;

    EXIT_WHEN(argc < 2,
        "%s requires at least one argument\n", argv[0]
    );

    /* get the path the program was started in */
    EXIT_WHEN(!getcwd(starting_path, PATH_MAX),
        "could not get the current directory"
    );

    number_of_files = read_args(&files_to_process, &settings, argc, argv);
    outcome = settings.execute(settings, number_of_files, starting_path, files_to_process);
    free(files_to_process);

    EXIT_WHEN(HAS_ERROR(outcome),
        "%s", outcome.description
    );

    return EXIT_SUCCESS;
}
