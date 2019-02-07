/**
 * \file motor.h
 *
 * A simple motor control class.
 *
 * ## Example usage
 *
 *     #include "motor.h"
 *
 *     MOTOR m;
 *
 *     int main () {
 *         DDRD |= _BV(PORTD4);
 *         motor_new(&m, &PORTD, &OCR0A, PORTD4, false);
 *         while (1) {
 *             motor_forward(&m, 127);
 *             _delay_ms(1000);
 *             motor_stop(&m);
 *             _delay_ms(1000);
 *             motor_reverse(&m, 127);
 *             _delay_ms(1000);
 *             motor_stop(&m);
 *             _delay_ms(1000);
 *     }
 *
 */
#ifndef _motor_h
#define _motor_h

#include <stdint.h>
#include <stdbool.h>

typedef struct MOTOR {
    uint8_t  *port,         //!< Port data register to which motor is attached
             *speed_ocr,    //!< OCR register that manages motor speed
             dir_pin;       //!< Pin on `port` that manages motor direction

    bool     running,       //!< `true` if motor is running
             reversed;      //!< `true` if motor is reversed
} MOTOR;

/**
 * Initialize a new `MOTOR` object
 */
void motor_new(MOTOR *motor, uint8_t *port, uint8_t *speed_ocr,
        uint8_t dir_pin, bool reversed);

/**
 * Stop the motor. This sets the corresponding OCR register to `0`
 */
void motor_stop(MOTOR *motor);

/**
 * Start the motor spinning forward at `speed`
 */
void motor_forward(MOTOR *motor, uint8_t speed);

/**
 * Start the motor spinning in reverse at `speed`
 */
void motor_reverse(MOTOR *motor, uint8_t speed);

#endif // _motor_h
