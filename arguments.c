#include "arguments.h"
#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>

#define MIN_THREADS 1u

#define ARGUMENT_XMAP(X) \
    X(FLAG_HELP, 'h', \
        "view this help") \
    X(FLAG_FILENAME_PREFIX, 'P', \
        "set a string to prefix renamed files") \
    X(FLAG_FILENAME_SUFFIX, 'S', \
        "set a string to suffix renamed files") \
    X(FLAG_THREAD_NUM, 't', \
        "[WIP] set a number of threads to use") \
    X(FLAG_DRY_RUN, 'd', \
        "dry run, don't modify files but show changes") \
    X(FLAG_CONFIRMATION, 'p', \
        "confirm each file change" ) \
    X(FLAG_DIR_AS_FILE, 'F', \
        "[WIP] treat arguments as individual files" ) \
    X(FLAG_COPY, 'C', \
        "[WIP] instead of renaming files, make a copy with the new name, " \
        "outputs to specified directory")

enum program_arguements {
    #define ENUMERATE(a, b, _) a = b,
        ARGUMENT_XMAP(ENUMERATE)
    #undef ENUMERATE
};

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

void usage(FILE *stream, const char *progname) {
    fprintf(stream,"%s usage:\n", progname);

    #define PRINT(_, b, c) fprintf(stream, "    -%c    %s\n", b, c);
        ARGUMENT_XMAP(PRINT)
    #undef PRINT
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
