#ifndef STR_APPEND_H
#define STR_APPEND_H

#include <stddef.h>

int str_append(char *buffer,
               const size_t current_len,
               const size_t max_len,
               const char *fmt, ...);

#endif /* STR_APPEND_H */
