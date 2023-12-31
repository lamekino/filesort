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

static int ensure_args(int num_needed, int total, int pos) {
    if (num_needed + pos >= total) {
        return -1;
    }

    return num_needed;
}

static int verify_number(char *s, int min, int max) {
    int n = atoi(s);

    if (n == 0) return -1;
    if (n > 1 && n < min) return -2;
    if (n > 1 && n > max) return -3;

    return n;
}

static enum program_arguements resolve_arguemnt(char flag) {
    switch (flag) {
    #define CASE(label, ch, _) case ch: return label;
        ARGUMENT_XMAP(CASE)
    #undef CASE
    }

    return -1;
}

static int argument_data_from_flag(settings_t *settings, char flag,
        struct argument_data *data) {
    int args_needed = 0;

    switch (resolve_arguemnt(flag)) {
    case FLAG_THREAD_NUM:
        args_needed = 1;
        data->type = NUM;
        data->num = (struct number_argument) {
            .max = MAX_THREADS,
            .min = MIN_THREADS,
            .field = (int *) &(settings->num_threads)
        };
        break;
    case FLAG_FILENAME_PREFIX:
        args_needed = 1;
        data->type = STR;
        data->str = &(settings->prefix);
        break;
    case FLAG_FILENAME_SUFFIX:
        args_needed = 1;
        data->type = STR;
        data->str = &(settings->suffix);
        break;
    case FILE_FILENAME_APPEND:
        args_needed = 1;
        data->type = STR;
        data->str = &(settings->appendix);
        break;
    case FLAG_DIR_AS_FILE:
        settings->run = &run_on_files;
        break;
    case FLAG_CONFIRMATION:
        settings->operation = &confirm_rename;
        break;
    case FLAG_COPY_FILES:
        settings->operation = &copy_file;
        break;
    case FLAG_DRY_RUN:
        settings->operation = &dry_rename;
        break;
    case FLAG_RECURSIVE:
        settings->use_recursion = 1;
        break;
    case FLAG_TERMINATOR:
        settings->use_flag_terminator = 1;
        break;
    case FLAG_HELP:
        data->type = HELP;
        break;
    default:
        return -1;
    }

    return args_needed;
}

static status_t set_flag(const struct argument_data *data,
        char *argv[], int index) {
    status_t status = STATUS_NORMAL;
    int verified = 0;

    switch (data->type) {
    case NONE:
        break;
    case HELP:
        usage(stdout, argv[0]);
        status = STATUS_SKIP;
        break;
    case STR:
        *data->str = argv[index + 1];
        break;
    case NUM:
        verified =
            verify_number(argv[index + 1], data->num.min, data->num.max);
        if (verified < 0) {
            CREATE_STATUS_ERR(status,
                    "invaild number '%s' for flag '%s' (min: %d, max: %d)",
                    argv[index + 1], argv[index], data->num.min,
                    data->num.max);
            return status;
        }

        *(data->num.field) = verified;
        break;
    default:
        ASSERT(0 && "unreachable");
        break;
    }
    return status;
}

status_t handle_flag(int *pos, int argc, char *argv[], settings_t *settings) {
    const int idx = *pos;
    int args_needed = 0, additional_args = 0;

    status_t status = STATUS_NORMAL;
    struct argument_data data = {0};

    args_needed = argument_data_from_flag(settings, argv[idx][1], &data);
    if (args_needed < 0) {
        usage(stderr, argv[0]);
        CREATE_STATUS_ERR(status,
                "unknown flag: '%s'\n", argv[idx]);
        return status;
    }

    additional_args = ensure_args(args_needed, argc, idx);
    if (additional_args < 0) {
        CREATE_STATUS_ERR(status,
                "argument required for flag '%s'", argv[idx]);
        return status;
    }

    *pos += additional_args + 1;
    return set_flag(&data, argv, idx);
}
