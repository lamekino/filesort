#ifndef APPLY_SETTINGS_H
#define APPLY_SETTINGS_H

#include "error_handling.h"

struct settings;

typedef status_t (*apply_func)(const struct settings *,
                               const int,
                               const char *,
                               char **);


status_t apply_settings(const struct settings *settings,
                        const int number_of_files,
                        const char *starting_path,
                        char **file_list);
#endif /* APPLY_SETTINGS_H */
