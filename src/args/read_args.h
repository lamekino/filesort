#ifndef __READ_ARGS_H
#define __READ_ARGS_H

#include "../error_handling.h"
#include "../settings.h"

status_t read_args(int *number_of_files,
                   char ***file_list,
                   settings_t *settings,
                   int argc,
                   char *argv[]);

#endif
