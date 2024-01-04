#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H

#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]

#ifdef DEBUG
#include <assert.h>
#define ASSERT(p) assert((p))
#else
#define ASSERT(_)
#endif /* DEBUG */

#define END_ENUM(name) name##_END

typedef union {
    enum {
        NORMAL,
        SKIP,
        FAILED,
        FATAL_NO_MEM,
        END_ENUM(status_t)
    } id;
    char *description;
} status_t;

#define STATUS_ERR_BUF_SIZE 256

#define STATUS_NORMAL ((status_t) {0})
#define STATUS_SKIP   ((status_t) { .id = SKIP })
#define STATUS_FAILED ((status_t) { .id = FAILED })
#define STATUS_NO_MEM ((status_t) { .id = FATAL_NO_MEM })

#define HAS_ERROR(s)     ((s).id >= END_ENUM(status_t))
#define IS_FATAL_ERR(s)  ((s).id == FATAL_NO_MEM)
#define HAS_FAILED(s)    ((s).id == FAILED)
#define IS_NORMAL(s)     ((s).id == NORMAL)
#define IS_SKIPPED(s)    ((s).id == SKIP)

status_t create_status_err(const char *fmt, ...);

#endif
