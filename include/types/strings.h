#ifndef STR_APPEND_H
#define STR_APPEND_H

#include <stddef.h>

int str_append(char *buffer, size_t current_len, size_t max_len,
        const char *fmt, ...);

size_t
path_concat(char *dest, const char *dirname, const char *fname);


#endif /* STR_APPEND_H */
