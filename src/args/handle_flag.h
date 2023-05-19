#ifndef __HANDLE_FLAG_H
#define __HANDLE_FLAG_H

#include "util/error_handling.h"
#include "util/settings.h"

status_t handle_flag(int *index,
                     int argc,
                     char *argv[],
                     settings_t *settings);

#endif
