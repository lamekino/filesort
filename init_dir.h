#ifndef __INIT_DIR_H
#define __INIT_DIR_H
#include <dirent.h>

int init_dir(const char *relative_path, DIR **dir_p);
#endif
