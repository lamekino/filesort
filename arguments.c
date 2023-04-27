#include "arguments.h"
#include "error_handling.h"
#include "usage.h"

#include <stdio.h>
#include <stdlib.h>

#define MIN_THREADS 1u

typedef char flag_t;

/* returns the amount of args process + the flag itself */
static int ensure_args(flag_t flag, const char *type, int num_needed,
        int total, int pos) {
    EXIT_WHEN(num_needed + pos > total,
        "-%c requires %d %s arguments but was given %d",
        flag, num_needed, type, total - pos
    );

    return num_needed + 1;
}

static int verify_number(char *num, char *purpose, int min, int max) {
    int n = atoi(num);
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

    return n;
}

int handle_flag(int index, int argc, char *argv[], struct user_settings *state) {
    const char *flag = argv[index];
    int args_parsed = 1;

    switch (flag[1]) {
    case FLAG_THREAD_NUM: {
        args_parsed =
            ensure_args(FLAG_THREAD_NUM, "number", 1, argc, index);

        state->num_threads =
            verify_number(argv[index + 1], "thread", 1, -1);
    } break;
    case FLAG_FILENAME_PREFIX: {
        args_parsed =
            ensure_args(FLAG_FILENAME_PREFIX, "string", 1, argc, index);

        state->prefix = argv[index + 1];
    } break;
    case FLAG_FILENAME_SUFFIX: {
        args_parsed =
            ensure_args(FLAG_FILENAME_SUFFIX, "string", 1, argc, index);

        state->suffix = argv[index + 1];
    } break;
    case FLAG_CONFIRMATION: {
        state->use_confirm = 1;
    } break;
    case FLAG_DRY_RUN: {
        state->dry_run = 1;
    } break;
    case FLAG_HELP:
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    default:
        fprintf(stderr, "Unknown flag: '%s'\n", flag);
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    return args_parsed;
}
