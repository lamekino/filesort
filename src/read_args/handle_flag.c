#include "read_args/handle_flag.h"

#include <stdlib.h>

#include "types/error.h"
#include "types/settings.h"

#include "read_args/parse_flag.h"
#include "read_args/usage.h"

static int
verify_number(char *s, int min, int max) {
    int n = atoi(s);

    if (n == 0) return -1;
    if (n > 1 && n < min) return -2;
    if (n > 1 && n > max) return -3;

    return n;
}

static union error
set_flag(const struct argument_meta *data,
        char *argv[], int index, size_t param_count) {
    (void) param_count;
    int verified = 0;

    switch (data->type) {
    case NONE:
        break;
    case HELP:
        usage(stdout, argv[0]);
        return SKIP_LEVEL;
    case STR:
        *data->str = argv[index + 1];
        break;
    case USE_FILE:
        return SPECIAL_LEVEL;
    case NUM:
        verified =
            verify_number(argv[index + 1], data->num.min, data->num.max);
        if (verified < 0) {
            return create_fatal_err(
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

    return SUCCEED_LEVEL;
}


union error
handle_flag(int *pos, int argc, char *argv[], struct settings *settings) {
    const int idx = *pos;
    struct argument_meta data = {0};
    int args_needed = parse_flag(settings, argv, idx, &data);

    if (args_needed < 0) {
        ASSERT(args_needed != UNIMPLEMENTED_FLAG);

        usage(stderr, argv[0]);
        return create_fatal_err("unknown flag: '%s'\n", argv[idx]);
    }

    if (idx + args_needed >= argc) {
        return create_fatal_err("argument required for flag '%s'", argv[idx]);
    }

    *pos += args_needed + 1;
    return set_flag(&data, argv, idx, args_needed);
}
