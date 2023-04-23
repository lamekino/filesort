#include "cli.h"
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
        "set a number of threads to use") \
    X(FLAG_DRY_RUN, 'd', \
        "dry run") \
    X(FLAG_CONFIRMATION, 'p', \
        "confirm each file change" )

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
        "-%c requires %d %s Preferences but was given %d",
        flag, num_needed, type, total - pos
    );

    return num_needed + 1;
}

void usage(FILE *stream, const char *progname) {
    fprintf(stream,"%s usage:\n", progname);

    #define PRINT(_, b, c) fprintf(stream, "\t-%c\t%s\n", b, c);
        ARGUMENT_XMAP(PRINT)
    #undef PRINT
}

int handle_flag(int index, int argc, char *argv[], struct Preferences *state) {
    const char *flag = argv[index];
    int num_parsed = 1;

    switch (flag[1]) {
    case FLAG_THREAD_NUM: {
        num_parsed = ensure_args(FLAG_THREAD_NUM, "number", 1, argc, index);

        unsigned int num_threads = atoi(argv[index + 1]);

        EXIT_WHEN(num_threads == 0,
                "error parsing number of threads: %s", argv[index + 1]);
        EXIT_WHEN(num_threads < MIN_THREADS,
                "%d is an invaild number of threads (minimum: %d)",
                num_threads, MIN_THREADS);

        state->num_threads = num_threads;
    } break;
    case FLAG_FILENAME_PREFIX: {
        num_parsed = ensure_args(FLAG_FILENAME_PREFIX, "string", 1, argc, index);

        state->prefix = argv[index + 1];
    } break;
    case FLAG_FILENAME_SUFFIX: {
        num_parsed = ensure_args(FLAG_FILENAME_SUFFIX, "string", 1, argc, index);

        state->suffix = argv[index + 1];
    } break;
    case FLAG_CONFIRMATION: {
        state->use_confirm = 1;
    } break;
    case FLAG_HELP:
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    default:
        fprintf(stderr, "Unknown flag: '%s'\n", flag);
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    return num_parsed;
}
