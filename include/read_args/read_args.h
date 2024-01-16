#ifndef READ_ARGS_H
#define READ_ARGS_H

#include "types/error.h"
#include "types/settings.h"

union error
read_args(size_t *number_of_files, char ***file_list,
        struct settings *settings, int argc, char *argv[]);

#endif /* READ_ARGS_H */
