#include <stdint.h>
#include <stdbool.h>

#include "motor.h"

void motor_new(MOTOR *motor,
        volatile uint8_t *port,
        volatile uint8_t *speed_ocr,
        uint8_t dir_pin,
        bool reversed) {
    motor->port      = port;
    motor->speed_ocr = speed_ocr;
    motor->dir_pin   = dir_pin;
    motor->running   = false;
    motor->reversed  = reversed;
}

void motor_stop(MOTOR *motor) {
    *(motor->speed_ocr) = 0;
    motor->running = false;
}

void motor_forward(MOTOR *motor, uint8_t speed) {
    motor->running = true;
    *(motor->speed_ocr) = speed;

    if (motor->reversed) {
        *(motor->port) &= ~(motor->dir_pin);
    } else {
        *(motor->port) |= (motor->dir_pin);
    }
}

void motor_reverse(MOTOR *motor, uint8_t speed) {
    motor->running = true;
    *(motor->speed_ocr) = speed;
    *(motor->port) &= ~(motor->dir_pin);

    if (motor->reversed) {
        *(motor->port) |= (motor->dir_pin);
    } else {
        *(motor->port) &= ~(motor->dir_pin);
    }
}
