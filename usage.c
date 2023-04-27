#include "usage.h"
#include "arguments.h"

#include <stdio.h>

void usage(FILE *stream, const char *progname) {
    fprintf(stream,"%s usage:\n", progname);

    /* prints out the list of predefined arguments */
    #define PRINT(_, b, c) fprintf(stream, "    -%c    %s\n", b, c);
        ARGUMENT_XMAP(PRINT)
    #undef PRINT
}

