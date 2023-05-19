#include <stdio.h>

int dry_rename(const char *src, const char *dest) {
    return printf("would rename '%s' -> '%s'\n", src, dest);
}
