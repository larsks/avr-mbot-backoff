#ifndef _unittest_h
#define _unittest_h

#include <stdio.h>
#include <stdlib.h>

#define REPORT_SUCCESS {\
    GPIOR0 = 0xFF; \
    GPIOR1 = 0xFF; \
    }

#define REPORT_FAILURE {\
    GPIOR0 = 0xFF; \
    GPIOR1 = 0x00; \
    }

#define ASSERT(expr, msg) if (!(expr)) { \
        fprintf(stderr, "* FAILED: "); \
        fprintf(stderr, msg); \
        fprintf(stderr, "\n"); \
        REPORT_FAILURE; \
        exit(1); \
    }

#endif // _unittest_h
