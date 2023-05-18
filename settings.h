#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "error_handling.h"
#include <stdlib.h>

struct settings;

/* see: apply_changes.h */
typedef status_t (*applicator)(const struct settings,
                               const int,
                               const char *,
                               char **);

typedef int (*transformer)(const char *, const char *);

typedef struct settings {
    size_t num_threads;

    char *prefix;
    char *suffix;

    unsigned int use_recursion:1;
    unsigned int use_flag_terminator:1;

    transformer transform_file;
    applicator execute;
} settings_t;
#endif
