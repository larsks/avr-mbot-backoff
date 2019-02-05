#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <util/delay.h>

#include "millis.h"
#include "distance.h"

#define T_MEASURE_START 100

volatile uint8_t t_measure = T_MEASURE_START;
volatile timer_t echo_start, echo_end, echo_duration;

void measure_begin() {
    TIMSK2 |= _BV(OCIE2A);
    TIFR2 |= _BV(OCF2A);
    OCR2A = 128;
    
    PCMSK1 |= _BV(PCINT9);
    PCICR |= _BV(PCIE1);
}

// fires every 1024us, or approximately every 1 ms.
ISR(TIMER2_COMPA_vect) {
    static volatile uint8_t state;

    if (! t_measure--) {
        t_measure = T_MEASURE_START;
        state = 1;
    }

    switch(state) {
        case 0:
            break;

        case 1:
            USDDR |= USPIN;
            USPORTREG |= USPIN;
            state = 2;
            break;

        case 2:
            USPORTREG &= ~(USPIN);
            USDDR &= ~(USPIN);
            state = 0;
            break;
    }
}

ISR(PCINT1_vect) {
    if (USPORTREG & USPIN) {
        echo_start = micros();
        echo_end = 0;
    } else {
        echo_end = micros();
        echo_duration = (echo_end - echo_start);
    }
}
