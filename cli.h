#ifndef __CLI_H
#define __CLI_H
#include <stdio.h>

struct user_settings {
    size_t num_threads;
    char *prefix;
    char *suffix;
    int use_confirm:1;
    int dry_run:1;
};

void usage(FILE *stream, const char *progname);
int handle_flag(int index, int argc, char *argv[], struct user_settings *state);
#endif
