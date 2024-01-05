#ifndef OPERATIONS_H
#define OPERATIONS_H

typedef int (*fileop_func)(const char *, const char *);

int confirm_rename(const char *orig_name, const char *new_name);
int copy_file(const char *src, const char *dest);
int dry_rename(const char *src, const char *dest);

#endif /* OPERATIONS_H */
