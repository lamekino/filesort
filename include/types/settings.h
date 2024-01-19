#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdlib.h>
#include "ops/ops.h"

struct settings;

/*
 * FIXME: this is a god object
 */
struct settings {
    size_t num_threads;

    char *prefix;
    char *suffix;
    char *appendix;

    unsigned int use_recursion:1;
    unsigned int use_flag_terminator:1;

    fileop operation;
};

#endif /* SETTINGS_H */
