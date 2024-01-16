#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "read_args.h"
#include "process_as_directories.h"
#include "error_handling.h"
#include "settings.h"

/*
 * TODO: factor out of main.c
 */
int report_errors(const status_t *s) {
    if (!HAS_ERROR(*s)) {
        return EXIT_SUCCESS;
    }

    if (IS_FATAL_ERR(*s)) {
        fprintf(stderr, "fatal memory error! you need more RAM!!\n");
        return EXIT_FAILURE;
    }

    if (s->description != NULL) {
        fprintf(stderr, "%s\n", s->description);
        free(s->description);
    }

    /* NOTE: s->description and s->errno are mutually exclusive right now */
    /* TODO: rename status_t to union status */
#if 0
    if (s->errno > 0) {
        fprintf(stderr, "errno(%d): %s\n", s->errno, strerror(s->errno));
    }
#endif

    return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    char starting_path[PATH_MAX + 1];

    size_t number_of_files = 0;
    char **file_list = NULL;

    settings_t settings = {0};
    status_t outcome = STATUS_NORMAL;

    if (argc < 2) {
        fprintf(stderr, "%s requires at least one argument\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* get the path the program was started in */
    if (!getcwd(starting_path, PATH_MAX)) {
        fprintf(stderr, "could not get the current directory");
        return EXIT_FAILURE;
    };

    outcome =
        read_args(&number_of_files, &file_list, &settings, argc, argv);
    if (IS_NORMAL(outcome)) {
        outcome = process_as_directories(&settings, file_list, number_of_files);
    }

    free(file_list);
    return report_errors(&outcome);
}
