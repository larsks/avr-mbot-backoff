#ifndef _distance_h
#define _distance

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#include "millis.h"

#ifndef USDDR
#define USDDR     (DDRC)            //!< DDR register for ultrasonic sensor
#endif

#ifndef USPORTREG
#define USPORTREG (PORTC)           //!< PORT register for ultrasonic sensor
#define USPINREG  (PINC)            //!< PIN register for ultrasonic sensor
#endif

#ifndef USPIN
#define USPIN     (_BV(PORTC1))     //!< Data pin for ultrasonic sensor
#endif

extern volatile bool measure_valid;
void measure_begin(void);
timer_t measure_value();

#endif // _distance
