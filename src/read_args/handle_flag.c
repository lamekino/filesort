#include "read_args/handle_flag.h"
#include "read_args/arguments.h"
#include "read_args/usage.h"

#include "runners/apply_changes.h"
#include "util/error_handling.h"
#include "transform/handlers.h"
#include "util/settings.h"

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

status_t handle_flag(int *pos,
                     int argc,
                     char *argv[],
                     settings_t *settings) {
    status_t status = STATUS_NORMAL;
    int index = *pos;
    int args_parsed = 1;

    char **string_to_set = NULL;

    size_t *number_to_set = NULL;
    int min_set_num = 0, max_set_num = 0;

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
    case FLAG_HELP: {
        usage(stdout, argv[0]);
        status = STATUS_SKIP;
    } break;
    default: {
        usage(stderr, argv[0]);
        CREATE_STATUS_ERR(status,
                "Unknown flag: '%s'\n", argv[index]);
    } break;
    }

    /* FIXME: this is the worst looking code ever */
    if (args_parsed < 0) {
        CREATE_STATUS_ERR(status,
                "argument required for flag '%s'", argv[index]);
        return status;
    }

    if (string_to_set != NULL) {
        *string_to_set = argv[index + 1];
    }

    if (number_to_set != NULL) {
        int verified =
            verify_number(argv[index + 1], min_set_num, max_set_num);

        if (verified < 0) {
            CREATE_STATUS_ERR(status,
                    "invaild number '%s' for flag '%s' (min: %d, max: %d)",
                    argv[index + 1], argv[index], min_set_num, max_set_num);
            return status;
        }
        *number_to_set = verified;
    }

    *pos += args_parsed;

    return status;
}
