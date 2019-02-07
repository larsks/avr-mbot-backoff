#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "millis.h"
#include "distance.h"

#define T_MEASURE_START 100

volatile uint8_t t_measure = T_MEASURE_START;
volatile timer_t echo_start,
         echo_end,
         echo_duration;

void measure_begin() {
    TIMSK2 |= _BV(OCIE2A);
    TIFR2 |= _BV(OCF2A);
    OCR2A = 128;
    
    PCMSK1 |= _BV(PCINT9);
    PCICR |= _BV(PCIE1);
}

timer_t measure_value() {
    timer_t _value = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        _value = echo_duration;
	}

    return _value;
}


/**
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
        _delay_us(20);
        USPORTREG &= ~(USPIN);
        USDDR &= ~(USPIN);
    }
}

ISR(PCINT1_vect) {
    static uint8_t counter = 0;

    if (USPINREG & USPIN) {
        echo_start = micros();
        echo_end = 0;
    } else {
        echo_end = micros();
        echo_duration = (echo_end - echo_start);
    }
}
