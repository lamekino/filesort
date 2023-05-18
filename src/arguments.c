#include "arguments.h"
#include "apply_changes.h"
#include "error_handling.h"
#include "usage.h"
#include "transform_file.h"
#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NOTE: placeholders */
#define MIN_THREADS 1u
#define MAX_THREADS 32u

/* returns the amount of args process + the flag itself */
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

static int handle_flag(int index,
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

status_t read_args(int *number_of_files,
                   char ***file_list,
                   settings_t *settings,
                   int argc,
                   char *argv[]) {
    status_t status = STATUS_NORMAL;
    int file_count = 0;
    int adx = 1;

    while (adx < argc) {
        char **file_list_resize = NULL;

        if (!settings->use_flag_terminator && argv[adx][0] == '-') {
            int args_parsed = handle_flag(adx, argc, argv, settings);

            if (args_parsed <= 0) {
                CREATE_STATUS_ERR(status,
                        "invalid argument for flag '%s'", argv[adx]);
                return status;
            }
            adx += args_parsed;
            continue;
        }

        file_count++;
        file_list_resize = realloc(*file_list, file_count * sizeof(*file_list));

        if (file_list_resize == NULL) {
            CREATE_STATUS_ERR(status,
                    "could not allocate memory for file list");
            return status;
        }

        /* set the value of the resized list and make the original pointer point
         * to it */
        file_list_resize[file_count - 1] = argv[adx];
        *file_list = file_list_resize;

        adx++;
    }

    *number_of_files = file_count;
    return status;
}
