#ifndef __RECURSE_DIRECTORY_H
#define __RECURSE_DIRECTORY_H
#include "util/settings.h"
#include <sys/stat.h>
#include <sys/types.h>

int recurse_directory(const settings_t *settings,
                      const char *filename,
                      const struct stat stat_info);

#endif
