#include "arguments.h"
#include "usage.h"

#include "../apply_changes.h"
#include "../transform_file.h"

#include <stdlib.h>

/* NOTE: placeholders */
#define MIN_THREADS 1u
#define MAX_THREADS 32u

static int ensure_args(int num_needed, int total, int pos) {
    if (num_needed + pos >= total) {
        return -1;
    }

    return num_needed + 1;
}

static int verify_number(char *num, int min, int max) {
    int n = atoi(num);
    if (n == 0) return -1;
    if (n > -1 && n < min) return -2;
    if (n > -1 && n > max) return -3;

    return n;
}

int handle_flag(int index,
                int argc,
                char *argv[],
                settings_t *settings) {
    int args_parsed = 1;

    char **string_to_set = NULL;

    size_t *number_to_set = NULL;
    int min_set_num, max_set_num;

    switch (argv[index][1]) {
    case FLAG_THREAD_NUM: {
        args_parsed = ensure_args(1, argc, index);

        min_set_num = MIN_THREADS;
        max_set_num = MAX_THREADS;
        number_to_set = &(settings->num_threads);
    } break;
    case FLAG_FILENAME_PREFIX: {
        args_parsed = ensure_args(1, argc, index);
        string_to_set = &(settings->prefix);
    } break;
    case FLAG_FILENAME_SUFFIX: {
        args_parsed = ensure_args(1, argc, index);
        string_to_set = &(settings->suffix);
    } break;
    case FLAG_DIR_AS_FILE: {
        settings->execute = &apply_on_files;
    } break;
    case FLAG_CONFIRMATION: {
        settings->transform_file = &confirm_rename;
    } break;
    case FLAG_COPY_FILES: {
        settings->transform_file = &copy_file;
    } break;
    case FLAG_DRY_RUN: {
        settings->transform_file = &dry_rename;
    } break;
    case FLAG_RECURSIVE: {
        settings->use_recursion = 1;
    } break;
    case FLAG_TERMINATOR: {
        settings->use_flag_terminator = 1;
    } break;
    case FLAG_HELP:
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    default:
        fprintf(stderr, "Unknown flag: '%s'\n", argv[index]);
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    if (string_to_set != NULL && args_parsed > 1) {
        *string_to_set = argv[index + 1];
    }

    if (number_to_set != NULL && args_parsed > 1) {
        size_t verified =
            verify_number(argv[index + 1], min_set_num, max_set_num);

        if (verified > 0) {
            *number_to_set = verified;
        }
        else {
            args_parsed = -1;
        }
    }

    return args_parsed;
}
