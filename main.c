#include <stdio.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "motor.h"
#include "distance.h"
#include "millis.h"
#include "serial.h"
#include "pid.h"

#define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define max(a,b) \
    ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define abs(a) \
    ({ __typeof__ (a) _a = (a); \
     _a >= 0 ? _a : (_a * -1); })

#define MOTORDDR  (DDRD)            //!< DDR register for motors
#define MOTORPORT (PORTD)           //!< PORT register for motors
#define MLEFTDIR  (_BV(PORTD4))     //!< Left motor direction pin
#define MLEFTPWM  (_BV(PORTD5))     //!< Left motor speed control
#define MRIGHTDIR (_BV(PORTD7))     //!< Right motor direction pin
#define MRIGHTPWM (_BV(PORTD6))     //!< Right motor speed control
#define MLEFTOCR  (OCR0A)           //!< Left motor OCR register
#define MRIGHTOCR (OCR0B)           //!< Right motor OCR register

#define BACKOFF_DISTANCE 30

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
    PID p;
    timer_t now, next_measure;
    char buf[80];
    uint8_t speed;

    setup();

    motor_new(&m1, &MOTORPORT, &MLEFTOCR, MLEFTDIR, false);
    motor_new(&m2, &MOTORPORT, &MRIGHTOCR, MRIGHTDIR, true);
    pid_new(&p, 20, -500, 5000, 5, 0, 1);
    pid_set_target(&p, BACKOFF_DISTANCE);

    while(1) {
        uint16_t cm;
        float output;
        uint8_t speed;

        cm = echo_duration/58.0;
        sprintf(buf, "duration: %u cm: %u", echo_duration, cm);
        serial_println(buf);
        continue;

        output = pid_update(&p, cm);

        speed = (int)(255 * (abs(output)/500.0));

        sprintf(buf, "cm: %d output: %d speed: %u", (int)cm, (int)output, speed);
        serial_println(buf);

        if (output >= 0) {
            motor_reverse(&m1, speed);
            motor_reverse(&m2, speed);
        } else if (output < 0) {
            motor_forward(&m1, speed);
            motor_forward(&m2, speed);
        }
    }
}
