#include <stdio.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "motor.h"
#include "distance.h"
#include "millis.h"
#include "serial.h"

#define MOTORDDR  (DDRD)            //!< DDR register for motors
#define MOTORPORT (PORTD)           //!< PORT register for motors
#define MLEFTDIR  (_BV(PORTD4))     //!< Left motor direction pin
#define MLEFTPWM  (_BV(PORTD5))     //!< Left motor speed control
#define MRIGHTDIR (_BV(PORTD7))     //!< Right motor direction pin
#define MRIGHTPWM (_BV(PORTD6))     //!< Right motor speed control
#define MLEFTOCR  (OCR0A)           //!< Left motor OCR register
#define MRIGHTOCR (OCR0B)           //!< Right motor OCR register

uint16_t counter;


void setup() {
    //! Configure motor pins as outputs.
    MOTORDDR |= MLEFTDIR | MRIGHTDIR | MLEFTPWM | MRIGHTPWM;

    TCCR0A = _BV(WGM00)  | _BV(WGM01) |  // Enable fast PWM (WGM = 0b011)
             _BV(COM0A1) | _BV(COM0B1); // Enable PWM output on OC0A and OC0B

    //! Select clk/64 prescaler (CS = 0b011)
    TCCR0B = _BV(CS01)  | _BV(CS00);

    serial_begin();
    init_millis();

    sei();
}

int main() {
    MOTOR m1,
          m2;
    timer_t now, next_measure;
    distance_t distance;
    char buf[40];

    setup();

    motor_new(&m1, &MOTORPORT, &MLEFTOCR, MLEFTDIR, false);
    motor_new(&m2, &MOTORPORT, &MRIGHTOCR, MRIGHTDIR, true);

    while(1) {
        sprintf(buf, "millis: %lu", millis());
        serial_println(buf);
        _delay_ms(1000);
    }
}
