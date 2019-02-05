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

#define BACKOFF_DISTANCE 25

void setup() {
    //! Configure motor pins as outputs.
    MOTORDDR |= MLEFTDIR | MRIGHTDIR | MLEFTPWM | MRIGHTPWM;

    TCCR0A = _BV(WGM00)  | _BV(WGM01) |  // Enable fast PWM (WGM = 0b011)
             _BV(COM0A1) | _BV(COM0B1); // Enable PWM output on OC0A and OC0B

    //! Select clk/64 prescaler (CS = 0b011)
    TCCR0B = _BV(CS01)  | _BV(CS00);

    init_millis();
    measure_begin();
    serial_begin();

    sei();
}

int main() {
    MOTOR m1,
          m2;
    timer_t now, next_measure;
    char buf[40];

    setup();

    motor_new(&m1, &MOTORPORT, &MLEFTOCR, MLEFTDIR, true);
    motor_new(&m2, &MOTORPORT, &MRIGHTOCR, MRIGHTDIR, false);

    while(1) {
        uint8_t cm;

        cm = echo_duration/58;

        if (cm < BACKOFF_DISTANCE-1) {
            motor_forward(&m1, 127);
            motor_forward(&m2, 127);
            PORTB &= ~_BV(PORTB5);
        } else if (cm > BACKOFF_DISTANCE+1) {
            motor_reverse(&m1, 127);
            motor_reverse(&m2, 127);
            PORTB &= ~_BV(PORTB5);
        } else {
            motor_stop(&m1);
            motor_stop(&m2);
            PORTB |= _BV(PORTB5);
        }
    }
}
