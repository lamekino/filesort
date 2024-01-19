#ifndef COLLECT_FILENAMES_H
#define COLLECT_FILENAMES_H

#include "types/error.h"
#include "types/multistack.h"

union error
collect_filenames(struct multistack *ms, char **dir_list, size_t len);

#endif /* COLLECT_FILENAMES_H */
