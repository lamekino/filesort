#ifndef OPERATIONS_H
#define OPERATIONS_H

int confirm_rename(const char *orig_name, const char *new_name);
int copy_file(const char *src, const char *dest);
int dry_rename(const char *src, const char *dest);

#endif /* OPERATIONS_H */
