#ifndef PROCESS_AS_DIRECTORIES_H
#define PROCESS_AS_DIRECTORIES_H

#include "types/settings.h"
#include "types/error.h"

union error
process_as_directories(const struct settings *settings,
        char **dirs, size_t len);

#endif /* PROCESS_AS_DIRECTORIES_H */
