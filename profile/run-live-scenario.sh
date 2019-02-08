#!/bin/sh

if [[ $# -ne 3 ]]; then
    echo "specify P I D" >&2
    exit 2
fi

P=$1
I=$2
D=$3

basename="$P-$I-$D"
logfile="log-${basename}.csv"

echo "setpoint raw output speed" > $logfile

make -C .. clean flash PID_P=$P PID_I=$I PID_D=$D
kst2 -f end -n 1000 $logfile --asciiFieldNames 1 -P 3 -y 1 -y 2 -y 3 -P 1 -y 4 &
python3 livelogger.py -o $logfile /dev/ttyUSB4
