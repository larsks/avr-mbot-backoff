#ifndef _pid_h
#define _pid_h

#include <stdint.h>

#include "millis.h"

typedef struct PID {
    float kP,
          kI,
          kD;

    float tI,
          tD;

    float setval,
          lastval,
          error,
          lasterror,
          output;

    float minval,
          maxval;

    timer_t interval,
            lastupdate;
} PID;

void pid_new(PID *pid, timer_t interval,
        float minval, float maxval,
        float kP, float kI, float kD);
void pid_set_target(PID *pid, float setval);
void pid_set_tuning(PID *pid, float kP, float kI, float kD);
void pid_set_limits(PID *pid, float minval, float maxval);
float pid_update(PID *pid, float curval);

#endif // _pid_h
