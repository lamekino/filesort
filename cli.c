#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>

typedef char flag_t;
#define NO_FLAG '\0'
#define FLAG_HELP 'h'
#define FLAG_THREAD_NUM 'j'
#define FLAG_FILENAME_PREFIX 'P'
#define FLAG_CONFIRMATION 'p'

#if 0
void ensure_args(flag_t flag, int needed, const int max, const char *type) {
    EXIT_WHEN(needed > max,
        "-%c requires a %s argument", flag, type
    );
}
#endif

void usage(FILE *stream, const char *progname) {
    fprintf(
        stream,
        "usage:"
        "\t%s [directories]\n"
        "\tmass rename files in a directory to their epoch creation date\n",
        progname);
}

int handle_flag(int index, const int argc, const char *argv[]) {
    const char *flag = argv[index];
    int flag_args = 0;

    switch (flag[1]) {
    case FLAG_THREAD_NUM:
        EXIT_WHEN(index + 1 > argc,
            "-%c requires a number argument", FLAG_THREAD_NUM
        );
        UNIMPLEMENTED;
    case FLAG_FILENAME_PREFIX:
        EXIT_WHEN(index + 1 > argc,
            "-%c requires a string argument", FLAG_FILENAME_PREFIX
        );
        UNIMPLEMENTED;
    case FLAG_CONFIRMATION:
        UNIMPLEMENTED;
    case FLAG_HELP:
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    case NO_FLAG:
    default:
        fprintf(stderr, "Unknown flag: '%s'\n", flag);
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    return flag_args;
}
