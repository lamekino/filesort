#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H

#define UNIMPLEMENTED do { \
        fprintf(stderr, "%s[%d]: '%s()' not implemented\n", \
                __FILE__, __LINE__, __FUNCTION__); \
    } while (0)

#define ASSERT(pred, msg) do { \
        if (!(pred)) { \
            fprintf(stderr, "%s[%d]: %s\n", __FILE__, __LINE__, msg); \
            exit(1); \
        } \
    } while (0)

#endif
