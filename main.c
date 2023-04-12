/* realpath requires _XOPEN_SOURCE >= 500 */
#define _XOPEN_SOURCE 500
/* PATH_MAX requires _POSIX_C_SOURCE == 1 */
#define _POSIX_C_SOURCE 1

#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h> /* PATH_MAX */
#include <string.h> /* strrchr */
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

void usage() {
    UNIMPLEMENTED;
}

int main(int argc, char *argv[]) {
    char starting_path[PATH_MAX];
    char working_path[PATH_MAX];
    int i;
    if (argc < 2) {
        usage();
        exit(1);
    }

    /* get the path the program was started in */
    getcwd(starting_path, PATH_MAX);
    for (i = 1; i < argc; i++) {
        const char *dirname = realpath(argv[i], working_path);
        struct dirent *dir_entry;

        DIR *dir = opendir(dirname);
        ASSERT(dir != NULL, "could not open directory");

        chdir(working_path);
        while ((dir_entry = readdir(dir)) != NULL) {
            struct stat statinfo;
            const char *orig_filename = dir_entry->d_name;
            const char *extension = strrchr(orig_filename, '.');

            /* skip hidden files, parent, current */
            if (orig_filename[0] == '.') {
                continue;
            }
            ASSERT(stat(orig_filename, &statinfo) == 0, "could not stat file");

            printf("[%s]: %s = %ld\n", dirname, orig_filename, statinfo.st_ctime);
        }
        chdir(starting_path);
        closedir(dir);
    }
    return 0;
}
