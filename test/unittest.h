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

#define ASSERT_GTE(expr1, expr2, msg) \
    { \
        __typeof__ (expr1) _expr1 = (expr1); \
        __typeof__ (expr2) _expr2 = (expr2); \
        if ((_expr1) < (_expr2)) { \
            fprintf(stderr, "* FAILED: "); \
            fprintf(stderr, msg); \
            fprintf(stderr, " (%d < %d)", _expr1, _expr2); \
            fprintf(stderr, "\n"); \
            REPORT_FAILURE; \
        } \
    }

#define ASSERT_EQUAL(expr1, expr2, msg) \
    { \
        __typeof__ (expr1) _expr1 = (expr1); \
        __typeof__ (expr2) _expr2 = (expr2); \
        if ((_expr1) != (_expr2)) { \
            fprintf(stderr, "* FAILED: "); \
            fprintf(stderr, msg); \
            fprintf(stderr, " (%d != %d)", _expr1, _expr2); \
            fprintf(stderr, "\n"); \
            REPORT_FAILURE; \
        } \
    }

#define ASSERT(expr, msg) if (!(expr)) { \
        fprintf(stderr, "* FAILED: "); \
        fprintf(stderr, msg); \
        fprintf(stderr, "\n"); \
        REPORT_FAILURE; \
    }

#endif // _unittest_h
