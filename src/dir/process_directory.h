#ifndef __PROCESS_DIRECTORY_H
#define __PROCESS_DIRECTORY_H
#include "util/error_handling.h"
#include "util/settings.h"
#include <stdlib.h>
#include <dirent.h>

status_t process_directory(const settings_t *settings,
                           DIR *dir,
                           size_t max_fname_len);
#endif
