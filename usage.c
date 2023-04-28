#include "usage.h"
#include "arguments.h"

#include <stdio.h>

void usage(FILE *stream, const char *progname) {
    fprintf(stream, "usage:\n");
    fprintf(stream,"%s [flags] <dir 1> <dir 2> <dir3> ...\n", progname);

    /* prints out the list of predefined arguments */
    #define PRINT(_, b, c) fprintf(stream, "    -%c    %s\n", b, c);
        ARGUMENT_XMAP(PRINT)
    #undef PRINT
}

