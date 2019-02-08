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

#define abs(a) \
    ({ __typeof__ (a) _a = (a); \
     _a >= 0 ? _a : (_a * -1); })

#define LEDPINREG PINB
#define LEDPORTREG PORTB
#define LEDPIN _BV(PORTB5)

#define MOTORDDR  (DDRD)            //!< DDR register for motors
#define MOTORPORT (PORTD)           //!< PORT register for motors
#define MLEFTDIR  (_BV(PORTD4))     //!< Left motor direction pin
#define MLEFTPWM  (_BV(PORTD5))     //!< Left motor speed control
#define MRIGHTDIR (_BV(PORTD7))     //!< Right motor direction pin
#define MRIGHTPWM (_BV(PORTD6))     //!< Right motor speed control
#define MLEFTOCR  (OCR0A)           //!< Left motor OCR register
#define MRIGHTOCR (OCR0B)           //!< Right motor OCR register

#define BACKOFF_DISTANCE_CM 20

MOTOR   m1,
        m2;
FastPID pid;

void setup() {
    //! Configure motor pins as outputs.
    MOTORDDR |= MLEFTDIR | MRIGHTDIR | MLEFTPWM | MRIGHTPWM;

    TCCR0A = _BV(WGM00)  | _BV(WGM01) |  // Enable fast PWM (WGM = 0b011)
             _BV(COM0A1) | _BV(COM0B1); // Enable PWM output on OC0A and OC0B

    //! Select clk/64 prescaler (CS = 0b011)
    TCCR0B = _BV(CS01)  | _BV(CS00);

    LEDPORTREG |= LEDPIN;

    init_millis();
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

    /**
     * The button on an mbot is wired to analog port A7. We can't 
     * perform digital reads on this port so we need to utilize
     * the microcontroller's ADC functionality.
     *
     * We track the button state in `last_ADCH1` and `cur_ADCH`.
     */
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
    pid_new(&pid, PID_P, PID_I, PID_D, 10, 16, true);
    pid_set_limits(&pid, -10000, 10000);

	ADMUX |= _BV(REFS0) | _BV(ADLAR) | 0b111;
	ADCSRA |= _BV(ADEN) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);

    while(1) {
        timer_t now,
                raw;
        int16_t output;
        uint8_t speed;

        now = millis();

        //! Set `ADSC` flag to enable an ADC conversion.
		ADCSRA |= _BV(ADSC);

        //! Wait for ADC value to become available.
		while ((ADCSRA & _BV(ADSC)));

        //! Record the current ADC result so that we can detect button
        //! state changes.
        cur_ADCH = ADCH;
		if (cur_ADCH && !last_ADCH) {
            LEDPINREG = LEDPIN;
			motors_enabled = !motors_enabled;
        }
        last_ADCH = cur_ADCH;

        if (now - last_measure >= 100) {
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
