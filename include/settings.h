#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdlib.h>
#include "error_handling.h"

struct settings;

/* see: apply_changes.h */
typedef status_t (*applicator_f)(const struct settings *,
                                 const int,
                                 const char *,
                                 char **);

typedef int (*operation_f)(const char *, const char *);

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

    operation_f operation;
    applicator_f run;
} settings_t;

#endif /* SETTINGS_H */
