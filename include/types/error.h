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

union error {
    struct {
        enum {
            NORMAL,
            SKIP,
            FAILED,
            FATAL_NO_MEM,
            END_STATUSES
        } id;
    };
    char *description;
};

#define STATUS_ERR_BUF_SIZE 256

#define STATUS_NORMAL ((union error) {0})
#define STATUS_SKIP   ((union error) { .id = SKIP })
#define STATUS_FAILED ((union error) { .id = FAILED })
#define STATUS_NO_MEM ((union error) { .id = FATAL_NO_MEM })

#define HAS_ERROR(s)     ((s).id >= END_STATUSES)
#define IS_FATAL_ERR(s)  ((s).id == FATAL_NO_MEM)
#define HAS_FAILED(s)    ((s).id == FAILED)
#define IS_NORMAL(s)     ((s).id == NORMAL)
#define IS_SKIPPED(s)    ((s).id == SKIP)

union error
create_status_err(const char *fmt, ...);

int
report_error(const union error *);

#endif /* ERROR_HANDLING_H */
