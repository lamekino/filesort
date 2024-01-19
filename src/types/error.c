#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "types/error.h"

static union error
va_create_fatal_err(const char *fmt, va_list ap) {
    char *dup = NULL;
    char buf[ERR_BUF_SIZE];

    if (vsnprintf(buf, ERR_BUF_SIZE, fmt, ap) < 0) {
        return ERROR_NO_MEM;
    }

    dup = strndup(buf, ERR_BUF_SIZE);
    if (dup == NULL) {
        return ERROR_NO_MEM;
    }

    return (union error) {
        .description = dup
    };
}

union error
create_fatal_err(const char *fmt, ...) {
    union error err;
    va_list args;

    va_start(args, fmt);
    err = va_create_fatal_err(fmt, args);
    va_end(args);

    return err;
}

void
fail(const char *fmt, ...) {
    union error err;
    va_list args;

    va_start(args, fmt);
    err = va_create_fatal_err(fmt, args);
    va_end(args);

    exit(report_error(&err));
}

int
report_error(const union error *e) {
    if (IS_NORM(*e)) {
        return EXIT_SUCCESS;
    }

    if (IS_LVL(*e, LEVEL_NO_MEM)) {
        fprintf(stderr, "fatal memory error! you need more RAM!!\n");
        return EXIT_FAILURE;
    }

    if (e->description != NULL) {
        fprintf(stderr, "%s\n", e->description);
        free(e->description);
    }

    return EXIT_FAILURE;
}
