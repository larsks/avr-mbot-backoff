/**
 * \file serial.h
 *
 * Simple interface to avr-libc UART functionality
 */
#ifndef _serial_h
#define _serial_h

#include <stdio.h>

extern FILE *uart;

void serial_begin();
void serial_putchar(char c);
int serial_putchar_f(char c, FILE *stream);
void serial_print(char *s);
void serial_println(char *s);

#endif // _serial_h
