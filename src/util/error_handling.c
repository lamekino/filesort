#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "error_handling.h"

status_t create_status_err(const char *fmt, ...) {
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
    return (status_t) {
        .description = dup
    };
}
