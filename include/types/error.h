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
    LEVEL_NONE,
    LEVEL_SKIP,
    LEVEL_FAILED,
    LEVEL_SPECIAL,
    LEVEL_NO_MEM,
    LEVEL_END
};

union error {
    enum error_level id;
    char *description;
};

#define ERR_BUF_SIZE 256

#define NO_ERROR ((union error) { .id = LEVEL_NONE })
#define ERROR_NO_MEM ((union error) { .id = LEVEL_NO_MEM })
#define ERROR(lvl) ((union error) { .id = (lvl) })

#define HAS_ERROR(s) ((s).id > LEVEL_END)
#define IS_OK(s) ((s).id == LEVEL_NONE)
#define IS_LVL(s, lvl) ((s).id == (lvl))

union error
create_fatal_err(const char *fmt, ...);

int
report_error(const union error *);

#endif /* ERROR_HANDLING_H */
