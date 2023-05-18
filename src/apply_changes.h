#ifndef __APPLY_CHANGES_H
#define __APPLY_CHANGES_H

#include "settings.h"
#include "error_handling.h"

/* TODO: make this a sensible number */
#define FNAME_MIN 32

status_t apply_on_dir(const settings_t settings,
                      const int number_of_files,
                      const char *starting_path,
                      char **file_list);

status_t apply_on_files(const settings_t settings,
                        const int number_of_files,
                        const char *starting_path,
                        char **file_list);
#endif
