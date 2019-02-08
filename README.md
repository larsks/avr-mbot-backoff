Configure an [mbot][] to follow an object at a fixed distance. This project makes use of the motors, the ultrasonic sensor, and the on-board button.

[mbot]: https://www.makeblock.com/steam-kits/mbot

## See also

The Doxygen documentation for this project is available online at <http://oddbit.com/avr-mbot-backoff>.

## Timers

- `TIMER0` is used for motor PWM
- `TIMER2` is used for millis/micros (`TOIE2`) and for driving the ultrasonic sensor (`OCIE2A`).
