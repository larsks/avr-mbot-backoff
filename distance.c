/**
 * \file distance.c
 *
 * Distance measurement using an [HC-SR04][] style sensor.
 *
 * [hc-sr04]: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
 */
#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "millis.h"
#include "distance.h"

#define T_MEASURE_START 100

/**
 * We want to start a new measurement roughly every 100ms.  The
 * `TIMER2_COMPA_vect` ISR fires about once/ms, so we decrement this
 * variable each time the ISR fires and when we reach zero start a new
 * measurement.
 */
volatile uint8_t t_measure = T_MEASURE_START;
volatile timer_t
         echo_start,    //!< micros() at start of sensor response
         echo_end,      //!< micros() at end of sensor response
         echo_duration, //!< duration of sensor response in ms
         _acc[3];

volatile bool measure_valid = false;

void measure_begin() {
    /**
     * TIMER2 is already being used by the `millis()`/`micros()`
     * implementation (and TIMER0 is used to drive the PWM signal for
     * the motors). Rather than consuming the last remaining timer, we
     * hook up an ISR to the compare match interrupt.
     */
    TIMSK2 |= _BV(OCIE2A);
    TIFR2 |= _BV(OCF2A);

    OCR2A = 128;
    
    /**
     * We process the return signal from the sensor using a pin change
     * interrupt handler. The handler is triggered twice for each
     * response: once on the rising edge of the signal, once on the
     * falling edge.
     */
    PCMSK1 |= _BV(PCINT9);
    PCICR |= _BV(PCIE1);
}

timer_t measure_value() {
    timer_t _value = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if ((_acc[0] < _acc[1] && _acc[1] < _acc[2]) || (_acc[2] < _acc[1] && _acc[1] < _acc[0])) 
			_value = _acc[1]; 

		else if ((_acc[1] < _acc[0] && _acc[0] < _acc[2]) || (_acc[2] < _acc[0] && _acc[0] < _acc[1])) 
			_value = _acc[0]; 

		else
			_value = _acc[2]; 
	}

    return _value;
}

/**
 * TIMER2 compare match interrupt handler.  This runs about 1/ms and
 * is responsible for scheduling measurements from the ultrasonic
 * sensor.
 *
 * When it is time to trigger a new measurement, we send a 10us high
 * pulse to the sensor, then configure the data pin as an input.
 *
 * ## Timing diagram
 *
 *        >10uS pulse
 *        |---|
 *        |   |
 *     ---+   +----
 *
 *                sensor sends ultrasonic pulses
 *                |-| |-| |-| |-| |-| |-| |-| |-|
 *                | | | | | | | | | | | | | | | |
 *     -----------+ +-+ +-| +-+ +-+ +-+ +-+ +-+ +---
 *
 *
 *     sensor sends return pulse to microcontroller |---------------------|
 *     that is propportional to distance            |                     |
 *     ---------------------------------------------+                     +---
 *
 */
ISR(TIMER2_COMPA_vect) {
    if (! t_measure--) {
        t_measure = T_MEASURE_START;
        USDDR |= USPIN;
        USPORTREG |= USPIN;
        _delay_us(10);
        USPORTREG &= ~(USPIN);
        USDDR &= ~(USPIN);
    }
}

/**
 * Pin change interrupt handler. This is triggered on the rising and
 * falling edge of the response from the sensor. We record the value
 * of `micros()` at each point, and then calculate the difference.
 */
ISR(PCINT1_vect) {
    static uint8_t counter = 2;

    if (USPINREG & USPIN) {
        echo_start = micros();
        echo_end = 0;
    } else {
        echo_end = micros();
        echo_duration = (echo_end - echo_start);
        _acc[counter--] = echo_duration;

        if (!counter) {
            counter = 2;
            measure_valid = true;
        }
    }
}
