#include "apply_settings.h"
#include "applications.h"

#include "settings.h"
#include "error_handling.h"

status_t apply_settings(const settings_t *settings,
                        const int number_of_files,
                        const char *starting_path,
                        char **file_list) {
    applicator_f default_run = &run_on_dirnames;

    if (settings->run == NULL) {
        return default_run(settings, number_of_files, starting_path, file_list);
    }

    return settings->run(settings, number_of_files, starting_path, file_list);
}
