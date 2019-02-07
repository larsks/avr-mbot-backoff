/**
 * \file serial.c
 */
#include <stdio.h>
#include <avr/io.h>

#ifndef BAUD
//! Selected serial baud rate. This must be set before including
//! `util/setbaud.h` because it is used in that file to calculate timings.
#define BAUD 9600
#endif

#include <util/setbaud.h>
#include "serial.h"

FILE _uart = FDEV_SETUP_STREAM(serial_putchar_f, NULL, _FDEV_SETUP_WRITE);
FILE *uart = &_uart;

//! Configure uart
void serial_begin() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
    UCSR0B = _BV(RXEN0)  | _BV(TXEN0);  // Enable RX and TX

    stdout = stderr = uart;
}

//! Output a single character. This waits for any in progress transmission
//! to complete before putting a new character into `UDR0`.
void serial_putchar(char c) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

int serial_putchar_f(char c, FILE *stream) {
    if (c == '\n')
        serial_putchar('\r');

    serial_putchar(c);
    return 0;
}

//! Print a string to the serial port
void serial_print(char *s) {
    while (*s) serial_putchar(*s++);
}

//! Print a string to the serial port, followed by a CR/LF pair
void serial_println(char *s) {
    serial_print(s);
    serial_putchar('\r');
    serial_putchar('\n');
}
