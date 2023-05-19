#ifndef __HANDLE_FLAG_H
#define __HANDLE_FLAG_H

#include "error_handling.h"
#include "settings.h"

status_t handle_flag(int *index,
                     int argc,
                     char *argv[],
                     settings_t *settings);

#endif
