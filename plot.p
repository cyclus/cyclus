#!/usr/bin/gnuplot

set terminal png size 400,300 enhanced font "Helvetica,20"
set output 'plot.png'
set xrange [0:]
set yrange [0:]
plot 'pred' using 1:2 title 'Predator' with linespoints, \
	 'prey' using 1:2 title 'Prey' with linespoints