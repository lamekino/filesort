#ifndef __USER_SETTINGS_H
#define __USER_SETTINGS_H

#include <stdlib.h>

struct user_settings {
    size_t num_threads;

    char *prefix;
    char *suffix;

    unsigned int use_confirm:1;
    unsigned int use_recursion:1;

    int (*transform_file)(const char*, const char*);
};
#endif
