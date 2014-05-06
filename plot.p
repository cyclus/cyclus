#!/usr/bin/gnuplot

set terminal png size 400,300 enhanced font "Helvetica,20"
set output 'plot.png'
plot 'prey' using 1:2 title 'Prey' with linespoints , \
      'pred' using 1:2 title 'Predator' with linespoints