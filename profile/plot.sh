#!/bin/sh

data=$1
output=$2
title=$3

gnuplot <<EOF
set datafile separator ","
set title "$title"
set ylabel "us"
set y2label "speed"
set term svg
set output "$output"
plot "$data" using 1 title "setpoint", "" using 2 title "raw", \
	"" using 3 title "output", \
	"" using 4 with lines title "speed" axis x1y2;
EOF

