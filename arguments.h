#ifndef __ARGUMENTS_H
#define __ARGUMENTS_H
#include <stdio.h>
#include "user_settings.h"

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
    X(FLAG_RECURSIVE, 'R', \
        "[WIP] be recursive - go into directories" ) \
    X(FLAG_COPY_FILES, 'c', \
        "copies files instead of renaming them") \

enum program_arguements {
    #define ENUMERATE(label, flag, _) label = flag,
        ARGUMENT_XMAP(ENUMERATE)
    #undef ENUMERATE
};

int read_args(char ***file_list,
              struct user_settings *settings,
              int argc,
              char *argv[]);
#endif
