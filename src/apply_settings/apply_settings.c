#include "apply_settings.h"
#include "applications.h"

#include "settings.h"
#include "error_handling.h"

status_t apply_settings(const settings_t *settings,
                        const int number_of_files,
                        const char *starting_path,
                        char **file_list) {
    const apply_func fallback = &run_on_dirnames;

    if (settings->apply == NULL) {
        return fallback(settings, number_of_files, starting_path, file_list);
    }

    return settings->apply(settings, number_of_files, starting_path, file_list);
}
