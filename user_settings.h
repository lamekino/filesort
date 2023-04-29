#ifndef __USER_SETTINGS_H
#define __USER_SETTINGS_H

#include <stdlib.h>

/* TODO: move this to its own file */
struct user_settings {
    size_t num_threads;

    char *prefix;
    char *suffix;

    int use_confirm:1;
    int dry_run:1;

    int (*transform_file)(const char*, const char*);
};
#endif
