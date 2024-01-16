#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdlib.h>
#include "operations.h"

struct settings;

/*
 * FIXME: this is a god object
 */
typedef struct settings {
    size_t num_threads;

    char *prefix;
    char *suffix;
    char *appendix;

    unsigned int use_recursion:1;
    unsigned int use_flag_terminator:1;

    fileop_func operation;
} settings_t;

#endif /* SETTINGS_H */
