#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#define STATIC_ASSERT(COND, MSG) \
    typedef char static_assertion_##MSG[(COND) ? 1 : -1]

#ifdef DEBUG
#include <assert.h>
#define ASSERT(p) assert((p))
#else
#define ASSERT(_)
#endif /* DEBUG */

enum error_level {
    LEVEL_SUCCESS,
    LEVEL_SKIP,
    LEVEL_FAILED,
    LEVEL_SPECIAL,
    LEVEL_NO_MEM,
};

union error {
    enum error_level level;
    char *description;
};

#define ERR_BUF_SIZE 256

#define SUCCEED_LEVEL ((union error) { .level = LEVEL_SUCCESS })
#define SKIP_LEVEL ((union error) { .level = LEVEL_SKIP })
#define FAILED_LEVEL ((union error) { .level = LEVEL_FAILED })
#define SPECIAL_LEVEL ((union error) { .level = LEVEL_SPECIAL })
#define ERROR_NO_MEM ((union error) { .level = LEVEL_NO_MEM })

#define HAS_ERROR(e) ((e).level >= LEVEL_NO_MEM)
#define IS_NORM(e) ((e).level == LEVEL_SUCCESS)
#define IS_SPECIAL(e) ((e).level == LEVEL_SPECIAL)
#define IS_LVL(e, id) ((e).level == (id))

union error
create_fatal_err(const char *fmt, ...);

void
fail(const char *fmt, ...);

int
report_error(const union error *);

#ifdef DEBUG
#define REPORT_MISSING_IMPL(e) \
    do { \
        if (!HAS_ERROR((e))) { \
            (e) = create_fatal_err("%s: got unimplemented error level %ud", \
                    __FUNCTION__, e.level); \
        } \
    } while (0)
#else
#define REPORT_MISSING(_)
#endif

#endif /* ERROR_HANDLING_H */
