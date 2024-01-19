#ifndef PROCESS_AS_FILES_H
#define PROCESS_AS_FILES_H

#include "types/settings.h"
#include "types/error.h"

union error
process_as_files(const struct settings *settings, char **files, size_t len);

#endif /* PROCESS_AS_FILES_H */
