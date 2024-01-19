#ifndef RENAME_FILES_H
#define RENAME_FILES_H

#include "types/settings.h"
#include "types/error.h"
#include "types/multistack.h"

union error
rename_files(const struct settings *settings, struct multistack *contents);

#endif /* RENAME_FILES_H */
