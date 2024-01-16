#include <stdio.h>
#include <ctype.h>

int confirm_rename(const char *src, const char *dest) {
    char user_input[64];

    printf("rename '%s' to '%s'? [y/N] ", src, dest);
    if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
        printf("invalid input, skipping...");
        return 0;
    }

    if (tolower(user_input[0]) == 'y') {
        printf("renamed '%s' ->'%s'\n", src, dest);
        return rename(src, dest);
    }

    printf("skipping '%s'...\n", src);
    return 0;
}

