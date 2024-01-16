#ifndef PROCESS_AS_DIRECTORIES_H
#define PROCESS_AS_DIRECTORIES_H

#include "settings.h"
#include "error_handling.h"

status_t
process_as_directories(const settings_t *settings,
        char **dirs, size_t len);

#endif /* PROCESS_AS_DIRECTORIES_H */
