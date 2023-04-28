#ifndef __CLI_H
#define __CLI_H
#include <stdio.h>

#define ARGUMENT_XMAP(X) \
    X(FLAG_HELP, 'h', \
        "view this help") \
    X(FLAG_FILENAME_PREFIX, 'P', \
        "set a string to prefix renamed files") \
    X(FLAG_FILENAME_SUFFIX, 'S', \
        "set a string to suffix renamed files") \
    X(FLAG_THREAD_NUM, 't', \
        "[WIP] set a number of threads to use") \
    X(FLAG_DRY_RUN, 'd', \
        "dry run, don't modify files but show changes") \
    X(FLAG_CONFIRMATION, 'p', \
        "confirm each file change" ) \
    X(FLAG_DIR_AS_FILE, 'F', \
        "[WIP] treat arguments as individual files" ) \
    X(FLAG_COPY_FILES, 'c', \
        "[WIP] copies files instead of renaming them") \

enum program_arguements {
    #define ENUMERATE(a, b, _) a = b,
        ARGUMENT_XMAP(ENUMERATE)
    #undef ENUMERATE
};

struct user_settings {
    size_t num_threads;

    char *prefix;
    char *suffix;

    int use_confirm:1;
    int dry_run:1;

    int (*transform_file)(const char*, const char*);
};

int read_args(char ***file_list,
              struct user_settings *settings,
              int argc,
              char *argv[]);
#endif
