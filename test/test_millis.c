#include <stdio.h>
#include <serial.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "millis.h"
#include "unittest.h"

int main() {
    serial_begin();

    millis_begin();
    sei();

    _delay_ms(100);
    ASSERT_GTE(millis(), 100, "millis() was too small");
    ASSERT_GTE(micros(), 100000, "micros() was too small");

    REPORT_SUCCESS;
}
