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
plotfile="plot-${basename}.svg"

make -C .. clean flash PID_P=$P PID_I=$I PID_D=$D
tio -b 9600 /dev/ttyUSB4 -l "$logfile"
sh plot.sh "$logfile" "$plotfile" "PID $P-$I-$D"
firefox "$plotfile"
