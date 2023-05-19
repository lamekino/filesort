#include "read_args/usage.h"
#include "read_args/arguments.h"

#include <stdio.h>

void usage(FILE *stream, const char *progname) {
    fprintf(stream, "usage:\n");
    fprintf(stream,"%s [flags] <dir 1> <dir 2> <dir3> ...\n", progname);

    /* prints out the list of predefined arguments */
    #define PRINT(_, flag, desc) fprintf(stream, "    -%c    %s\n", flag, desc);
        ARGUMENT_XMAP(PRINT)
    #undef PRINT
}

