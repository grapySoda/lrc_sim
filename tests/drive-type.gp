reset

set terminal postscript eps color enhanced size 5,3.5 font "Times-Roman" 22
set output 'drive-type.eps'

set ylabel 'Latency (ms)'
set xlabel 'Operation Number'

#set xrange [0:400]
#set yrange [0:20]
# set ytics 150

set border 3 back
set tics nomirror out scale 0.75

p "rand-write_lat.1.log" u 0:($2/1000000) notitle w lp
p "sim-rand-write_lat.1.log" u 0:($2/1000000) notitle w lp

set ylabel 'Fraction of Requests'
set xlabel 'Latency (ms)'

datafile1 = "rand-write_lat.1.log"
datafile2 = "sim-rand-write_lat.1.log"

#stats datafile1
#stats datafile2

#p \
#datafile1 u ($2/1000000):(1./STATS_records) smooth cumulative linewidth 4 title "Measured" w l, \
#datafile2 u ($2/1000000):(1./STATS_records) smooth cumulative linewidth 4 title "Simulated" w l