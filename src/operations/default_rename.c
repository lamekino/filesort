#include <stdio.h>

int
default_rename(const char *from, const char *to) {
    printf("renaming %s -> %s\n", from, to);
    return rename(from, to);
}
