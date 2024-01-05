#ifndef RECURSE_DIRECTORY_H
#define RECURSE_DIRECTORY_H
#include <sys/stat.h>
#include <sys/types.h>
#include "settings.h"

int recurse_directory(const settings_t *settings,
                      const char *filename,
                      const struct stat stat_info);

#endif /* RECURSE_DIRECTORY_H */
