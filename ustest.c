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

int main() {
    char buf[80];

    init_millis();
    serial_begin();
    measure_begin();
    sei();

    DDRB = 0b111110;
    PORTB = 0;

    while (1) {
        sprintf(buf, "d: %u", echo_duration);
        serial_println(buf);
    }
}
