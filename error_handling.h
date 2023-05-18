#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    enum {
        NORMAL,
        ERROR,
        SKIP,
    } id;
    char *description;
} status_t;

/* TODO: allow for printf strings in STATUS_ERR */
#define STATUS_ERR(msg) ((status_t) { .id = ERROR, .description = (msg) })
#define STATUS_OK ((status_t) {0})
#define STATUS_SKIP ((status_t) { .id = SKIP })

#define HAS_ERROR(s) (s.id == ERROR)
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
