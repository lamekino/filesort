#ifndef PROCESS_DIRECTORY_H
#define PROCESS_DIRECTORY_H
#include "settings.h"
#include <stdlib.h>
#include <dirent.h>

status_t process_directory(const settings_t *settings,
                           DIR *dir,
                           size_t max_fname_len);
#endif /* PROCESS_DIRECTORY_H */
