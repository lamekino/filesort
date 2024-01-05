#include "process_file/recurse_directory.h"

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "settings.h"
#include "init_dir.h"
#include "process_directory.h"

int recurse_directory(const settings_t *settings,
                      const char *filename,
                      const struct stat stat_info) {
    DIR *next_dir = NULL;
    int len = 0;

    /* not a directory -- do nothing */
    if (!(stat_info.st_mode & S_IFDIR)) {
        return 0;
    }

    /* don't use recursion, skip file */
    if (!settings->use_recursion) {
        fprintf(stderr, "skipping directory '%s'\n", filename);
        return 0;
    }

    /* the file is a direcatory and recursion is enabled */
    len = init_dir(filename, &next_dir);

    /* this is indirect recursion! */
    /* TODO: return this status */
    process_directory(settings, next_dir, len);
    closedir(next_dir);

    /* NOTE: S_IFDIR and IS_ISLNK are mutually exclusive, this will always be
     * parent */
    chdir("..");

    return 1;
}
