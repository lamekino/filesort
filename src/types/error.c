#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "types/error.h"

union error
create_status_err(const char *fmt, ...) {
    va_list args;
    char *dup = NULL;
    char buf[STATUS_ERR_BUF_SIZE];

    va_start(args, fmt);

    if (vsnprintf(buf, STATUS_ERR_BUF_SIZE, fmt, args) < 0) {
        return STATUS_NO_MEM;
    }

    dup = strndup(buf, STATUS_ERR_BUF_SIZE);
    if (dup == NULL) {
        return STATUS_NO_MEM;
    }

    va_end(args);
    return (union error) {
        .description = dup
    };
}

int
report_error(const union error *e) {
    if (!HAS_ERROR(*e)) {
        return EXIT_SUCCESS;
    }

    if (IS_FATAL_ERR(*e)) {
        fprintf(stderr, "fatal memory error! you need more RAM!!\n");
        return EXIT_FAILURE;
    }

    if (e->description != NULL) {
        fprintf(stderr, "%s\n", e->description);
        free(e->description);
    }

    return EXIT_FAILURE;
}
