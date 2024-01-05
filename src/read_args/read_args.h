#ifndef __READ_ARGS_H
#define __READ_ARGS_H

#include "util/error_handling.h"
#include "util/settings.h"

status_t read_args(size_t *number_of_files,
                   char ***file_list,
                   settings_t *settings,
                   int argc,
                   char *argv[]);

#endif
