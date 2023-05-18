#ifndef __TRANSFORM_FILE_H
#define __TRANSFORM_FILE_H
int confirm_rename(const char *orig_name, const char *new_name);
int copy_file(const char *src, const char *dest);
int dry_rename(const char *src, const char *dest);
#endif
