#include "read_args/handle_flag.h"
#include "read_args/arguments.h"
#include "read_args/usage.h"

#include "apply_settings/applications.h"
#include "util/error_handling.h"
#include "transform/handlers.h"
#include "util/settings.h"

#include <stdlib.h>

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

static int argument_data_from_flag(settings_t *settings, char *argv[],
        size_t idx, struct argument_meta *data) {
    const enum program_arguement arg = resolve_arguemnt(argv[idx][1]);

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
        ASSERT((arg < 0 || arg >= NUM_FLAGS) && "missing implementation");
        return -1;
    }
}

static int verify_number(char *s, int min, int max) {
    int n = atoi(s);

    if (n == 0) return -1;
    if (n > 1 && n < min) return -2;
    if (n > 1 && n > max) return -3;

    return n;
}

static status_t set_flag(const struct argument_meta *data,
        char *argv[], int index, size_t param_count) {
    (void) param_count;
    int verified = 0;

    switch (data->type) {
    case NONE:
        break;
    case HELP:
        usage(stdout, argv[0]);
        return STATUS_SKIP;
    case STR:
        *data->str = argv[index + 1];
        break;
    case NUM:
        verified =
            verify_number(argv[index + 1], data->num.min, data->num.max);
        if (verified < 0) {
            return create_status_err(
                    "invaild number '%s' for flag '%s' (min: %d, max: %d)",
                    argv[index + 1], argv[index], data->num.min,
                    data->num.max);
        }

        *(data->num.field) = verified;
        break;
    default:
        ASSERT(0 && "unreachable");
        break;
    }
    return STATUS_NORMAL;
}

status_t handle_flag(int *pos, int argc, char *argv[], settings_t *settings) {
    const int idx = *pos;
    struct argument_meta data = {0};
    int args_needed = argument_data_from_flag(settings, argv, idx, &data);

    if (args_needed < 0) {
        usage(stderr, argv[0]);
        return create_status_err("unknown flag: '%s'\n", argv[idx]);
    }

    if (idx + args_needed >= argc) {
        return create_status_err("argument required for flag '%s'", argv[idx]);
    }

    *pos += args_needed + 1;
    return set_flag(&data, argv, idx, args_needed);
}
