/**
 * \file main.c
 */
#include <stdio.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "motor.h"
#include "distance.h"
#include "millis.h"
#include "serial.h"
#include "FastPID.h"

/**
 * Return the absolute value of a numeric value.  This uses gcc's
 * [`__auto_type`](https://gcc.gnu.org/onlinedocs/gcc/Typeof.html)
 * keyword to avoid evaluating `expr` multiple times.
 */
#define abs(expr) \
    ({ __auto_type _a = (expr); \
     _a >= 0 ? _a : (_a * -1); })

#define LEDPINREG PINB              //!< LED input register (used for toggling)
#define LEDPORTREG PORTB            //!< LED port data register
#define LEDPIN _BV(PORTB5)          //!< LED pin

#define MOTORDDR  (DDRD)            //!< DDR register for motors
#define MOTORPORT (PORTD)           //!< PORT register for motors
#define MLEFTDIR  (_BV(PORTD4))     //!< Left motor direction pin
#define MLEFTPWM  (_BV(PORTD5))     //!< Left motor speed control
#define MRIGHTDIR (_BV(PORTD7))     //!< Right motor direction pin
#define MRIGHTPWM (_BV(PORTD6))     //!< Right motor speed control
#define MLEFTOCR  (OCR0A)           //!< Left motor OCR register
#define MRIGHTOCR (OCR0B)           //!< Right motor OCR register

//! Configure the mbot to stay this many cm away from obstacles
#define BACKOFF_DISTANCE_CM 20

MOTOR   m1,     //!< Controller for motor 1
        m2;     //!< Controller for motor 2
FastPID pid;    //!< PID controller (see <https://github.com/larsks/FastPID/>)

void setup() {
    /**
     * # Configure motors
     *
     * Configure motor pins as outputs (pwm and direction for each
     * motor).
     */
    MOTORDDR |= MLEFTDIR | MRIGHTDIR | MLEFTPWM | MRIGHTPWM;

    /**
     * Enable fast PWM mode on TIMER0 and enable PWM output on
     * `OC0A` and `OC0B`.
     */
    TCCR0A = _BV(WGM00)  | _BV(WGM01) |
             _BV(COM0A1) | _BV(COM0B1);

    /**
     * Configure clk/64 prescaler. The motor PWM frequency is 967Hz.
     * With the mcu running at 16Mhz, the /64 divider gives us a
     * frequency of 967.5625Hz (16000000/64/255) which is "close
     * enough".
     */
    TCCR0B = _BV(CS01)  | _BV(CS00);

    LEDPORTREG |= LEDPIN;

    millis_begin();
    measure_begin();
    serial_begin();

    sei();
}

int main() {
    timer_t last_measure = 0;

    /**
     * The backoff distance is specified in cm, but we need to convert
     * that into microseconds to match the units returned by the
     * `measure_value()` function. The velocity of sound through air
     * is approximately 340M/s, which comes to about about 29 us/cm.
     * Since the ultrasonic sensor is measuring the round-trip
     * distance of a pulse, we multiple our `BACKOFF_DISTANCE_CM`
     * value by 58.
     */
    int16_t target = BACKOFF_DISTANCE_CM * 58;

    //! Track button state so that we can detect a button press
    uint8_t last_ADCH = 1,
            cur_ADCH;

    /**
     * Pressing the button toggles `motors_enabled` on and off.  When
     * `motors_enabled` is false, the motor speed is fixed at 0.
     */
	bool motors_enabled = false;

    setup();

    motor_new(&m1, &MOTORPORT, &MLEFTOCR, MLEFTDIR, false);
    motor_new(&m2, &MOTORPORT, &MRIGHTOCR, MRIGHTDIR, true);
    pid_new(&pid, PID_P, PID_I, PID_D, 50, 16, true);
    pid_set_limits(&pid, -10000, 10000);

    /**
     * ## Configuring ADMUX
     *
     * The button on an mbot is wired to analog port `A7`. We can't 
     * perform digital reads on this port so we need to utilize
     * the microcontroller's ADC functionality.
     *
     * We configure `ADMUX` for `AREF` connected to 5v, we set `ADLAR`
     * so that we only need to read `ADCH` to detect a button press,
     * and we enable ADC on port `A7`.
     */
	ADMUX |= _BV(REFS0) | _BV(ADLAR) | 0b111;

    /**
     * ## Configuring ADCSRA
     *
     * The ADC requires an input clock frequency between 50kHz and
     * 200kHz. Since the mcu is running at 16Mhz, we enable the /128
     * prescaler by setting `ADPS0|ADPS1|ADPS2`, which gives gives the
     * ADC a clock frequency of 128kHz.
     *
     * We set the `ADEN` bit to enable ADC conversions.
     */
	ADCSRA |= _BV(ADEN) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);

    /**
     * ## Main loop
     */
    while(1) {
        timer_t now,
                raw;
        int16_t output;
        uint8_t speed;

        now = millis();

        /**
         * Read button state by setting `ADSC` to enable an ADC
         * conversion, then waiting for the ADC result to become
         * available.
         */
		ADCSRA |= _BV(ADSC);
		while ((ADCSRA & _BV(ADSC)));

        /**
         * Record the current ADC result so that we can watch for
         * high/low state changes.
         */
        cur_ADCH = ADCH;

        /**
         * Toggle the `motors_enabled` flag if we detect a low-to-high
         * transition (i.e., a button release).
         */
		if (cur_ADCH && !last_ADCH) {
            LEDPINREG = LEDPIN;
			motors_enabled = !motors_enabled;
        }
        last_ADCH = cur_ADCH;

        /**
         * We configured the PID controller with a 50Hz update
         * frequency, which means we need to take a new measurement
         * every 20ms.
         */
        if (now - last_measure >= 20) {
            last_measure = now;
            raw = measure_value();
            output = pid_step(&pid, target, raw);

            /**
             * We can't use the output from the PID controller
             * directly, so we use it to scale the motor speed
             * (which can be 0-255). The larger the value we get from
             * the PID controller, the faster the motors.
             */
            speed = (int)(255 * (abs(output)/10000.0));

            /**
             * Below a certain speed threshold, the motors just make a
             * whining sound without actually moving. Just set the
             * speed to 0 in this case.
             */
            if (speed < 10)
                speed = 0;

            /**
             * For analysis purposes, during each update of the PID controller
             * we print out the setpoint, the raw measurement from the
             * distance sensor, the output of the PID loop, and the
             * resulting speed.
             */
            printf("%d ", target);
            printf("%d ", raw);
            printf("%d ", output);
            printf("%u\n", speed);

            if (motors_enabled) {
                if (output > 0) {
                    motor_reverse(&m1, speed);
                    motor_reverse(&m2, speed);
                } else {
                    motor_forward(&m1, speed);
                    motor_forward(&m2, speed);
                }
            } else {
                motor_stop(&m1);
                motor_stop(&m2);
            }
        }
    }
}
