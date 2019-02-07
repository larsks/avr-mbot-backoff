#include <stdio.h>
#include <serial.h>
#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"
#include "unittest.h"

int main() {
    serial_begin();
    REPORT_SUCCESS;
}
