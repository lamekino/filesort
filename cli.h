#ifndef __CLI_H
#define __CLI_H
#include <stdio.h>

void usage(FILE *stream, const char *progname);
int handle_flag(int index, const int argc, const char *argv[]);
#endif
