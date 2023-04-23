#ifndef __PROCESS_DIRECTORY_H
#define __PROCESS_DIRECTORY_H
#include "cli.h"
#include <stdlib.h>
#include <dirent.h>

void process_directory(const struct Preferences *state, DIR *dir, size_t max_fname_len);
#endif
