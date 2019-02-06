#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <util/delay.h>

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

ISR(TIMER2_COMPA_vect) {
    static volatile uint8_t state;

    if (! t_measure--) {
        t_measure = T_MEASURE_START;
        state = 1;
    }

    switch(state) {
        case 0:
            PORTB &= ~_BV(PORTB4);
            break;

        case 1:
            PORTB |= _BV(PORTB4);
            USDDR |= USPIN;
            USPORTREG |= USPIN;
            _delay_us(20);
            USPORTREG &= ~(USPIN);
            USDDR &= ~(USPIN);
            state = 0;
            break;
    }
}

ISR(PCINT1_vect) {
    PINB = _BV(PORTB5);

    if (USPINREG & USPIN) {
        PORTB |= _BV(PORTB1);
        echo_start = micros();
        echo_end = 0;
    } else {
        PORTB &= ~_BV(PORTB1);
        echo_end = micros();
        echo_duration = (echo_end - echo_start);
    }
}
