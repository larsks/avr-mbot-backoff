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

    while (1) {
        sprintf(buf, "echo duration: %u", echo_duration);
        serial_println(buf);
    }
}
