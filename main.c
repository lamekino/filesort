/* realpath requires _XOPEN_SOURCE >= 500 */
#define _XOPEN_SOURCE 500
/* strnlen requires _POSIX_C_SOURCE >= 200809L */
#define _POSIX_C_SOURCE 200809L
/* TODO: make this a sensible number */
#define FNAME_MIN 32

#include "cli.h"
#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* strrchr */
#include <limits.h> /* PATH_MAX */
#include <dirent.h> /* readdir, opendir */
#include <time.h>   /* time_t */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * NOTE: requires the function to be ran while the program is in the same
 * directory as `filename`
 */
void get_new_filename(char *buffer,
                     size_t max_len,
                     const char *filename,
                     const char *prefix,
                     const struct stat *statinfo) {
    enum RenameProperties {
        HAS_EXTENSION = 0x1,
        HAS_DUPLICATES = 0x10,
        HAS_PREFIX = 0x100,
        HAS_EXTENSION_DUP_PREFIX = 0x111
    };
    unsigned int properties = 0;

    int num_duplicates = 0;
    int file_exists = 0;

    const time_t creation_time = statinfo->st_ctime;
    const char *extension = strrchr(filename, '.');

    properties |= extension != NULL;
    properties |= (prefix != NULL) << 2;

    while (file_exists || num_duplicates == 0) {
        switch (properties) {
        case HAS_EXTENSION:
            snprintf(buffer, max_len,
                    "%ld%s", creation_time, extension);
            break;
        case HAS_DUPLICATES:
            snprintf(buffer, max_len,
                    "%ld.%d", creation_time, num_duplicates);
            break;
        case HAS_PREFIX:
            UNIMPLEMENTED;
        case HAS_EXTENSION | HAS_DUPLICATES:
            snprintf(buffer, max_len,
                    "%ld.%d%s", creation_time, num_duplicates, extension);
            break;
        case HAS_EXTENSION | HAS_PREFIX:
            UNIMPLEMENTED;
        case HAS_EXTENSION_DUP_PREFIX:
            UNIMPLEMENTED;
        default: /* no extension + no duplicates */
            snprintf(buffer, max_len,
                    "%ld", creation_time);
            break;
        }

        file_exists = access(buffer, F_OK) == 0;
        num_duplicates++;
        properties |= HAS_DUPLICATES;
    }
}

void process_directory(const char *prefix, DIR *dir, size_t max_fname_len) {
    struct dirent *dir_entry = NULL;
    char confirm[64];
    while ((dir_entry = readdir(dir))) {
        struct stat statinfo;
        const char *old_filename = dir_entry->d_name;
        char *new_filename = NULL;
        /* skip hidden files, parent, current */
        if (old_filename[0] == '.') {
            continue;
        }

        EXIT_WHEN(stat(old_filename, &statinfo) < 0,
           "could not stat file '%s'", old_filename
        );

        new_filename = malloc(max_fname_len);
        EXIT_WHEN(new_filename == NULL,
           "could not allocate memory for filename."
        );

        get_new_filename(new_filename, max_fname_len, prefix, old_filename,
                &statinfo);

        printf("renaming '%s' -> '%s'\n", old_filename, new_filename);

        /*
         * FIXME: memory leak if this fails
         */
        EXIT_WHEN(rename(old_filename, new_filename) != 0,
                "could not rename file: %s", old_filename);
        free(new_filename);
    }
}

int main(const int argc, const char *argv[]) {
    char starting_path[PATH_MAX];
    char current_dir[PATH_MAX];
    char *filename_prefix = NULL;
    int idx, args_processed;
    if (argc < 2) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* get the path the program was started in */
    EXIT_WHEN(!getcwd(starting_path, PATH_MAX),
        "could not get the current directory"
    );

    for (idx = 1; idx < argc; idx++) {
        size_t new_filename_len = 0;
        DIR *dir = NULL;

        if (argv[idx][0] == '-') {
            args_processed = handle_flag(idx, argc, argv);
            continue;
        }

        realpath(argv[idx], current_dir);
        new_filename_len = PATH_MAX - strnlen(current_dir, PATH_MAX);
        dir = opendir(current_dir);

        EXIT_WHEN(new_filename_len < FNAME_MIN,
            "path '%s' too long to rename file", current_dir
        );
        EXIT_WHEN(dir == NULL,
            "could not open directory %s", current_dir
        );
        EXIT_WHEN(chdir(current_dir) != 0,
            "can't access directory '%s'", current_dir
        );

        process_directory(filename_prefix, dir, new_filename_len);
        closedir(dir);
        /* restore starting path */
        chdir(starting_path);
        args_processed++;
    }

    return EXIT_SUCCESS;
}
