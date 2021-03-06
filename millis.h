/**
 * \file millis.h
 */

#ifndef _millis_h
#define _millis_h

#include <stdint.h>

typedef uint32_t timer_t;

void millis_begin();
timer_t millis();
timer_t micros();

#endif // _millis_h
