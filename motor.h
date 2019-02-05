#ifndef _motor_h
#define _motor_h

#include <stdint.h>
#include <stdbool.h>

typedef struct MOTOR {
    uint8_t speed,
            dir;

    volatile uint8_t
            *port,
            *speed_ocr;
    uint8_t dir_pin;

    bool    running,
            reversed;
} MOTOR;

typedef enum MOTOR_DIRECTION {FORWARD, REVERSE} MOTOR_DIRECTION;

void motor_new(MOTOR *motor, volatile uint8_t *port, volatile uint8_t *speed_ocr, uint8_t dir_pin, bool reversed);
void motor_stop(MOTOR *motor);
void motor_forward(MOTOR *motor, uint8_t speed);
void motor_reverse(MOTOR *motor, uint8_t speed);

#endif // _motor_h
