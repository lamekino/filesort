#ifndef HANDLE_FLAG_H
#define HANDLE_FLAG_H

#include "types/error.h"
#include "types/settings.h"

union error
handle_flag(int *index, int argc, char *argv[], struct settings *settings);

#endif /* HANDLE_FLAG_H */
