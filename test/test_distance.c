#include <stdio.h>
#include <serial.h>
#include <avr/io.h>
#include <util/delay.h>
#include "distance.h"
#include "unittest.h"

int main() {
    serial_begin();
    measure_begin();

    REPORT_SUCCESS;
}
