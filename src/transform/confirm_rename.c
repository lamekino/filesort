#include <stdio.h>

int confirm_rename(const char *src, const char *dest) {
    char user_input[64];

    printf("rename '%s' to '%s'? [y/N] ", src, dest);
    fgets(user_input, 64, stdin);
    if (user_input[0] == 'y' || user_input[0] == 'Y') {
        printf("renamed '%s' ->'%s'\n", src, dest);
        return rename(src, dest);
    }

    printf("skipping '%s'...\n", src);
    return 0;
}

