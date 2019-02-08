/**
 * \file distance.h
 *
 * Distance measurement using an [HC-SR04][] style sensor.
 *
 * [hc-sr04]: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
 */
#ifndef _distance_h
#define _distance

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#include "millis.h"

#ifndef USDDR
//! data direction register associated with the hc-sr04
#define USDDR     (DDRC)            //!< DDR register for ultrasonic sensor
#endif

#ifndef USPORTREG
//! port (output) register associated with the hc-sr04
#define USPORTREG (PORTC)           //!< PORT register for ultrasonic sensor
//! pin (input) register associated with the hc-sr04
#define USPINREG  (PINC)            //!< PIN register for ultrasonic sensor
#endif

#ifndef USPIN
//! pin to which the sensor is attached
#define USPIN     (_BV(PORTC1))     //!< Data pin for ultrasonic sensor
#endif

//! becomes `true` when the first valid measurement is available
extern volatile bool measure_valid;

//! initialize the ultrasonic sensor
void measure_begin(void);

//! return the most recent measurement from the sensor
timer_t measure_value();

#endif // _distance
