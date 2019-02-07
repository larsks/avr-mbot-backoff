/**
 * \file unittest.h
 *
 * Defines macros for use in unit tests.
 */
#ifndef _unittest_h
#define _unittest_h

#include <stdio.h>
#include <stdlib.h>

//! set on GPIOR0 to indicate test completion
#define TEST_COMPLETE _BV(7)

//! set on GPIOR0 to indicate test success
#define TEST_SUCCESS _BV(0)

//! report successful completion of tests
#define REPORT_SUCCESS {\
    GPIOR0 = TEST_COMPLETE | TEST_SUCCESS; \
    }

//! report failed completion of tests
#define REPORT_FAILURE {\
    GPIOR0 = TEST_COMPLETE; \
    }

//! report test failure if `expr1` is not greather than or equal to `expr2`
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

//! report test failure if `expr1` is not equal to `expr2`
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

//! report test failure if `expr` does not evaluate to true
#define ASSERT(expr, msg) if (!(expr)) { \
        fprintf(stderr, "* FAILED: "); \
        fprintf(stderr, msg); \
        fprintf(stderr, "\n"); \
        REPORT_FAILURE; \
    }

#endif // _unittest_h
