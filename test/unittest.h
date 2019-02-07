#ifndef _unittest_h
#define _unittest_h

#include <stdio.h>
#include <stdlib.h>

#define TEST_COMPLETE _BV(7)
#define TEST_SUCCESS _BV(0)

#define REPORT_SUCCESS {\
    GPIOR0 = TEST_COMPLETE | TEST_SUCCESS; \
    }

#define REPORT_FAILURE {\
    GPIOR0 = TEST_COMPLETE; \
    }

#define ASSERT(expr, msg) if (!(expr)) { \
        fprintf(stderr, "* FAILED: "); \
        fprintf(stderr, msg); \
        fprintf(stderr, "\n"); \
        REPORT_FAILURE; \
        exit(1); \
    }

#endif // _unittest_h
