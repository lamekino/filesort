#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H

#ifdef DEBUG
#include <assert.h>
#define ASSERT(p) assert((p))
#else
#define ASSERT(_)
#endif /* DEBUG */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
#define CREATE_STATUS_ERR(s, msgs...) \
    do { \
        char *buf = malloc(STATUS_ERR_BUF_SIZE); \
        if (buf == NULL) { \
            s.id = FATAL_NO_MEM; \
            break; \
        }; \
        if (snprintf(buf, STATUS_ERR_BUF_SIZE, msgs) < 0) { \
            free(buf); \
            s.id = FATAL_NO_MEM; \
            break; \
        } \
        s.description = buf; \
    } while (0)

#define STATUS_NORMAL ((status_t) {0})
#define STATUS_SKIP   ((status_t) { .id = SKIP })
#define STATUS_FAILED ((status_t) { .id = FAILED })

#define HAS_ERROR(s)     ((s).id >= END_ENUM(status_t))
#define IS_FATAL_ERR(s)  ((s).id == FATAL_NO_MEM)
#define HAS_FAILED(s)    ((s).id == FAILED)
#define IS_NORMAL(s)     ((s).id == NORMAL)
#define IS_SKIPPED(s)    ((s).id == SKIP)

#define UNIMPLEMENTED \
    do { \
        fprintf(stderr, "%s: '%s()', line %d not implemented, exiting\n", \
                __FILE__, __FUNCTION__, __LINE__); \
        exit(EXIT_FAILURE); \
    } while (0)

#define WARNING(msgs...) \
    do { \
        int errsv = errno; \
        fprintf(stderr, "ERROR: %s[%d]: ", __FILE__, __LINE__); \
        fprintf(stderr, msgs); \
        if (errsv > 0) { \
            fprintf(stderr, "\n\t-> errno is '%s' (%d)", \
                    strerror(errsv), errsv); \
        } \
        fprintf(stderr, "\n"); \
    } while (0);

#endif
