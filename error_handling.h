#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef union {
    enum {
        NORMAL,
        SKIP,
        FAILED,

        /* newline + comment to empathize implementation detail */
        ID_UPPER_BOUND,
    } id;
    char *description;
} status_t;

/* TODO: allow for printf strings in STATUS_ERR */
#define USE_CREATE
#ifndef USE_CREATE
#define STATUS_ERR(msg) ((status_t) { .description = (msg) })
#else
#define STATUS_ERR_BUF_SIZE 256
#define CREATE_STATUS_ERR(s, msg...) \
    do { \
        /* there could be unknown consquences of this... */ \
        static char buf[STATUS_ERR_BUF_SIZE]; \
        snprintf(buf, STATUS_ERR_BUF_SIZE, msg); \
        s.description = buf; \
    } while (0)
#endif

#define STATUS_NORMAL ((status_t) {0})
#define STATUS_SKIP ((status_t) { .id = SKIP })
#define STATUS_FAILED ((status_t) { .id = FAILED })

#define HAS_ERROR(s) (s.id > ID_UPPER_BOUND)
#define HAS_FAILED(s) (s.id == FAILED)
#define IS_NORMAL(s) (s.id == NORMAL)
#define IS_SKIPPED(s) (s.id == SKIP)

#define PASS do { \
        fprintf(stderr, "%s[%d]: '%s()' not implemented\n", \
                __FILE__, __LINE__, __FUNCTION__); \
    } while (0)

#define UNIMPLEMENTED do { \
        fprintf(stderr, "%s: '%s()', line %d not implemented, exiting\n", \
                __FILE__, __FUNCTION__, __LINE__); \
        exit(EXIT_FAILURE); \
    } while (0)

#define EXIT_WHEN(pred, msgs...) do { \
        int errsv = errno; \
        if (!(pred)) { \
            break; \
        } \
\
        fprintf(stderr, "ERROR: %s[%d]: ", __FILE__, __LINE__); \
        fprintf(stderr, msgs); \
        if (errsv > 0) { \
            fprintf(stderr, "\n\t-> errno is '%s' (%d)\n", \
                    strerror(errsv), errsv); \
        } \
        else { \
            fprintf(stderr, "\n"); \
        } \
        exit(EXIT_FAILURE); \
    } while (0)
#endif
