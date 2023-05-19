#ifndef __APPLY_SETTINGS_H
#define __APPLY_SETTINGS_H

#include "util/error_handling.h"
#include "util/settings.h"

status_t apply_settings(const settings_t *settings,
                        const int number_of_files,
                        const char *starting_path,
                        char **file_list);
#endif
