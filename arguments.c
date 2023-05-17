#include "arguments.h"
#include "error_handling.h"
#include "usage.h"
#include "transform_file.h"
#include "user_settings.h"

#include <stdio.h>
#include <stdlib.h>

#define MIN_THREADS 1u

/* returns the amount of args process + the flag itself */
static int ensure_args(int num_needed, int total, int pos) {
    if (num_needed + pos >= total) {
        return -1;
    }
#if 0
    EXIT_WHEN(num_needed + pos > total,
        "-%c requires %d %s arguments but was given %d",
        flag, num_needed, type, total - pos
    );
#endif

    return num_needed + 1;
}

static int verify_number(char *num, int min, int max) {
    int n = atoi(num);
    if (n == 0) return -1;
    if (n > -1 && n < min) return -2;
    if (n > -1 && n > max) return -3;

#if 0
    EXIT_WHEN(n == 0,
        "error parsing number of %s in '%s'", purpose, num
    );
    EXIT_WHEN(n > -1 && n < min,
        "%d is an invaild number of %s (minimum: %d)",
        n, purpose, min
    );
    EXIT_WHEN(n > -1 && n > max,
        "%d is an invaild number of %s (maximum: %d)",
        n, purpose, max
    );
#endif

    return n;
}

static int handle_flag(int index,
                       int argc,
                       char *argv[],
                       struct user_settings *settings) {
    const char *flag = argv[index];
    int args_parsed = 1;
    void *field_to_set = NULL;

    switch (flag[1]) {
    case FLAG_THREAD_NUM: {
#if 0
        args_parsed =
            ensure_args(FLAG_THREAD_NUM, "number", 1, argc, index);

        settings->num_threads =
            verify_number(argv[index + 1], "thread", 1, -1);
#endif
        UNIMPLEMENTED;
    } break;
    case FLAG_FILENAME_PREFIX: {
        args_parsed = ensure_args(1, argc, index);
        field_to_set = &(settings->prefix);
    } break;
    case FLAG_FILENAME_SUFFIX: {
        args_parsed = ensure_args(1, argc, index);
        field_to_set = &(settings->suffix);
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
    case FLAG_HELP:
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    default:
        fprintf(stderr, "Unknown flag: '%s'\n", flag);
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    if (field_to_set != NULL && args_parsed > 1) {
        field_to_set = argv[index + 1];
    }

    return args_parsed;
}

int read_args(char ***file_list,
              struct user_settings *settings,
              int argc,
              char *argv[]) {
    int adx = 1;
    int number_of_files = 0;

    while (adx < argc) {
        char **file_list_resize = NULL;

        if (argv[adx][0] == '-') {
            int args_parsed = handle_flag(adx, argc, argv, settings);

            if (args_parsed <= 0) goto FLAG_ERROR;
            adx += args_parsed;
            continue;
        }

        number_of_files++;
        file_list_resize = realloc(*file_list, sizeof(*file_list) * number_of_files);
        if (file_list_resize == NULL) goto MEMORY_ERROR;

        /* set the value of the resized list and make the original pointer point
         * to it */
        file_list_resize[number_of_files - 1] = argv[adx];
        *file_list = file_list_resize;

        adx++;
    }

    return number_of_files;

/*
 * NOTE: in defense of using a goto:
 * 1) this function allocates the memory, it shouldn't be the caller's responsiblity to in an error
 * 2) it reduces the complexity of the code if we were to just return -1
 */
MEMORY_ERROR:
    if (*file_list != NULL) {
        free(*file_list);
    }
    EXIT_WHEN(1,
        "could not allocate memory for files"
    );
FLAG_ERROR:
    if (*file_list != NULL) {
        free(*file_list);
    }
    EXIT_WHEN(1,
        "invalid argument for flag '%s'", argv[adx]
    );
}
