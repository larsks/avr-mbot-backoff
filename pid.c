#include <stdint.h>

#include "pid.h"
#include "millis.h"

// interval is in ms
void pid_new(PID *pid, timer_t interval,
        float minval, float maxval,
        float kP, float kI, float kD) {

    pid->interval   = interval;
    pid->lastupdate = 0;
    pid->setval     = 0;
    pid->lastval    = 0;
    pid->error      = 0;
    pid->lasterror  = 0;
    pid->output     = 0;

    pid_set_tuning(pid, kP, kI, kD);
    pid_set_limits(pid, minval, maxval);
}

void pid_set_tuning(PID *pid, float kP, float kI, float kD) {
    float interval_in_seconds = ((float)pid->interval)/1000;

    pid->kP = kP;
    pid->kI = kI * interval_in_seconds;
    pid->kD = kD / interval_in_seconds;
}

void pid_set_limits(PID *pid, float minval, float maxval) {
    pid->minval = minval;
    pid->maxval = maxval;

    if (pid->tI > maxval)
        pid->tI = maxval;
    else if (pid->tI < minval)
        pid->tI = minval;

    if (pid->output > maxval)
        pid->output = maxval;
    else if (pid->output < minval)
        pid->output = minval;
}

void pid_set_target(PID *pid, float setval) {
    pid->setval = setval;
}

float pid_update(PID *pid, float curval) {
    float error,
          d_error,
          d_input, tI;

    timer_t now;

    now = millis();

    if (now - pid->lastupdate >= pid->interval) {
        error = (pid->setval - curval);
   
        pid->tI += (pid->kI * pid->error);
        if (pid->tI > pid->maxval)
            pid->tI = pid->maxval;
        else if (pid->tI < pid->minval)
            pid->tI = pid->minval;

        d_error = (error - pid->lasterror);
        d_input = (curval - pid->lastval);

        pid->output = (pid->kP * error) +
                      (pid->tI) -
                      (pid->kD * d_input);
        if (pid->output > pid->maxval)
            pid->output = pid->maxval;
        else if (pid->output < pid->minval)
            pid->output = pid->minval;

        pid->lastupdate = now;
        pid->lastval = curval;
    }

    return pid->output;
}
