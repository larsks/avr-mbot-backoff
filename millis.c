/**
 * \file millis.c
 *
 * Provide an analog of the Arduino `millis()` function. This implementation
 * uses `TIMER0` with a `/64` divider, which will allow it to operate with a
 * clock frequency of up to 16Mhz.
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "millis.h"

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )

// the prescaler is set so that timer ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER_OVERFLOW / 1000)

// the fractional number of milliseconds per timer overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile timer_t timer_overflow_count = 0;
volatile timer_t timer_millis         = 0;
uint8_t  timer_fract                  = 0;

ISR(TIMER2_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	timer_t m = timer_millis;
	uint8_t f = timer_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer_fract = f;
	timer_millis = m;
	timer_overflow_count++;
}

void init_millis() {
    TCCR2A = _BV(WGM20) | _BV(WGM21);   // fast pwm mode
    TCCR2B = _BV(CS22);                 // clk/64 prescaler
    TIMSK2 = _BV(TOIE2);
}

timer_t millis()
{
	unsigned long m;

	// disable interrupts while we read timer_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer_millis)
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m = timer_millis;
    }

	return m;
}

timer_t micros() {
	unsigned long m;
	uint8_t t;
	
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m = timer_overflow_count;
        t = TCNT2;

        if ((TIFR2 & _BV(TOV2)) && (t < 255))
            m++;
    }

	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}
