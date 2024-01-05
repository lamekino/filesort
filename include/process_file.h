#ifndef PROCESS_FILE_H
#define PROCESS_FILE_H
#include "error_handling.h"
#include "settings.h"

status_t process_file(const settings_t *settings,
                      const char *filename,
                      const size_t len);
#endif /* PROCESS_FILE_H */
