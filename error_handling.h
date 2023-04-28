#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PASS do { \
        fprintf(stderr, "%s[%d]: '%s()' not implemented\n", \
                __FILE__, __LINE__, __FUNCTION__); \
    } while (0)

#define UNIMPLEMENTED do { \
        fprintf(stderr, "%s[%d]: '%s()' not implemented, exiting\n", \
                __FILE__, __LINE__, __FUNCTION__); \
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
