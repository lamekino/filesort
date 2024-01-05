#include "read_args/parse_flag.h"
#include "apply_settings/applications.h"
#include "operations/operations.h"

/* NOTE: placeholders */
#define MIN_THREADS 1u
#define MAX_THREADS 32u

#define set_toggle(a, b) ((a) = (b)), 0

static int set_num(void *field, int min, int max,
                   struct argument_meta *data) {
    data->type = NUM;
    data->num = (struct number_argument) {
        .max = MAX_THREADS,
        .min = MIN_THREADS,
        .field = field
    };

    return 1;
}

static int set_str(char **field, struct argument_meta *data) {
    data->type = STR;
    data->str = field;

    return 1;
}

static enum program_arguement resolve_arguemnt(char flag) {
    switch (flag) {
    #define CASE(label, ch, _) case ch: return label;
        ARGUMENT_XMAP(CASE)
    #undef CASE
    }

    return -1;
}

static int apply_flag(settings_t *settings, enum program_arguement arg,
                      struct argument_meta *data) {
    switch (arg) {
    case FLAG_THREAD_NUM:
        return set_num(&settings->num_threads, MIN_THREADS, MAX_THREADS, data);
    case FLAG_FILENAME_PREFIX:
        return set_str(&settings->prefix, data);
    case FLAG_FILENAME_SUFFIX:
        return set_str(&settings->suffix, data);
    case FILE_FILENAME_APPEND:
        return set_str(&settings->appendix, data);
    case FLAG_DIR_AS_FILE:
        return set_toggle(settings->run, &run_on_files);
    case FLAG_CONFIRMATION:
        return set_toggle(settings->operation, &confirm_rename);
    case FLAG_COPY_FILES:
        return set_toggle(settings->operation, &copy_file);
    case FLAG_DRY_RUN:
        return set_toggle(settings->operation, &dry_rename);
    case FLAG_RECURSIVE:
        return set_toggle(settings->use_recursion, 1);
    case FLAG_TERMINATOR:
        return set_toggle(settings->use_flag_terminator, 1);
    case FLAG_HELP:
        return set_toggle(data->type, HELP);
    default:
        ASSERT(arg >= NUM_FLAGS && "missing implementation");
        return -1;
    }
}

int parse_flag(settings_t *settings, char **argv, size_t idx,
               struct argument_meta *data) {
    /*
     * TODO: create support for --long-flags
     */
    enum program_arguement arg_type = resolve_arguemnt(argv[idx][1]);
    return apply_flag(settings, arg_type, data);
}

