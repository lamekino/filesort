#ifndef READ_ARGS_H
#define READ_ARGS_H

#include "error_handling.h"
#include "settings.h"

status_t read_args(size_t *number_of_files,
                   char ***file_list,
                   settings_t *settings,
                   int argc,
                   char *argv[]);

#endif /* READ_ARGS_H */
