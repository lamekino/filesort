#ifndef __PROCESS_FILE_H
#define __PROCESS_FILE_H
#include "util/error_handling.h"
#include "util/settings.h"
#include <time.h>

status_t process_file(const settings_t *settings,
                      const char *filename,
                      const size_t len);
#endif
