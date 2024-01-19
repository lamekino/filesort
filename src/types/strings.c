#include <stdio.h>
#include <stdarg.h>

#include "ops/ops.h"
#include "types/strings.h"

int
str_append(char *buffer, size_t current_len, size_t max_len,
        const char *fmt, ...) {
    size_t chars_written;
    va_list ap;
    va_start(ap, fmt);

    /* these new values represent the current position in the buffer and the
     * new max length given from the current amount written to the buffer */
    char *buf_pos = buffer + current_len;
    size_t delta_len = max_len - current_len;

    chars_written = vsnprintf(buf_pos, delta_len, fmt, ap);

    va_end(ap);
    return chars_written;
}

size_t
path_concat(char *dest, const char *dirname, const char *fname) {
    return snprintf(dest, PATH_LEN, "%s/%s", dirname, fname);
}


