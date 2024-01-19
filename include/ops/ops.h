#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "limits.h"
#define PATH_LEN (PATH_MAX + 1)

typedef int (*fileop)(const char *, const char *);

int confirm_rename(const char *, const char *);
int copy_file(const char *, const char *);
int dry_rename(const char *, const char *);
int default_rename(const char *, const char *);

#endif /* OPERATIONS_H */
