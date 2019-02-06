/**
 * \file pid.c
 *
 * This is a port of Mike Matera's [FastPID][] code.  From Mike's `README`:
 *
 * > This PID controller is faster than alternatives for Arduino because it avoids
 * > expensive floating point operations. The PID controller is configured with
 * > floating point coefficients and translates them to fixed point internally. This
 * > imposes limitations on the domain of the coefficients. Setting the I and D
 * > terms to zero makes the controller run faster. The controller is configured to
 * > run at a fixed frequency and calling code is responsible for running at that
 * > frequency. The Ki and Kd parameters are scaled by the frequency to save time
 * > during the step() operation.
 *
 * [fastpid]: https://github.com/mike-matera/FastPID
 */

#ifndef _pid_h
#define _pid_h

#include <stdint.h>

#define INTEG_MAX    (INT32_MAX)
#define INTEG_MIN    (INT32_MIN)
#define DERIV_MAX    (INT16_MAX)
#define DERIV_MIN    (INT16_MIN)

#define PARAM_SHIFT  8
#define PARAM_BITS   16
#define PARAM_MAX    (((0x1ULL << PARAM_BITS)-1) >> PARAM_SHIFT) 
#define PARAM_MULT   (((0x1ULL << PARAM_BITS)) >> (PARAM_BITS - PARAM_SHIFT)) 

typedef struct PID {
    uint32_t _p, _i, _d;
    int64_t _outmax, _outmin;
    bool _cfg_err;

    int16_t _last_sp, _last_out;
    int64_t _sum;
    int32_t _last_err;
} PID;

/*
  A fixed point PID controller with a 32-bit internal calculation pipeline.
*/
void pid_new(PID *pid, float kp, float ki, float kd,
        float hz, int bits, bool sign);
bool pid_set_coefficients(PID *pid, float kp, float ki, float kd, float hz);
bool pid_set_output(PID *pid, int bits, bool sign);
bool pid_set_limits(PID *pid, int16_t min, int16_t max);
void pid_clear(PID *pid);
bool pid_configure(PID *pid, float kp, float ki, float kd,
        float hz, int bits, bool sign);
int16_t pid_step(PID *pid, int16_t sp, int16_t fb);

#endif // _pid_h
