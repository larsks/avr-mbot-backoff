#include <stdio.h>
#include <serial.h>
#include <avr/io.h>
#include "motor.h"
#include "unittest.h"

MOTOR _m;
MOTOR *m = &_m;

int main() {
    serial_begin();

    DDRD = _BV(PORTD4) | _BV(PORTD5);

    motor_new(m, &PORTD, &OCR0A, _BV(PORTD4), false);

    motor_forward(m, 127);
    ASSERT(m->running, "motor did not start");
    ASSERT_EQUAL(OCR0A, 127, "OCR0A did not have expected value");
    ASSERT(PORTD & _BV(PORTD4), "motor direction should be set");

    motor_stop(m);
    ASSERT(!m->running, "motor did not stop");
    ASSERT(OCR0A == 0, "OCR0A did not have expected value (0)");

    motor_reverse(m, 127);
    ASSERT(!(PORTD & _BV(PORTD4)), "motor direction should not be set");

    REPORT_SUCCESS;
}
