#ifndef APPLY_SETTINGS_H
#define APPLY_SETTINGS_H

#include "error_handling.h"
#include "settings.h"

status_t apply_settings(const settings_t *settings,
                        const int number_of_files,
                        const char *starting_path,
                        char **file_list);
#endif /* APPLY_SETTINGS_H */
