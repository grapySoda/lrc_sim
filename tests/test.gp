reset

set terminal postscript eps color enhanced size 5,3.5 font "Times-Roman" 22
set output 'test.eps'

set ylabel 'Fraction of Requests'
set xlabel 'Latency (ms)'

datafile1 = "test.log"
stats datafile1
p datafile1 u ($2/1000000):(1./STATS_records) smooth cumulative notitle w l

#datafile2 = "sim-rand-write_lat.1.log"
#stats datafile2
#p datafile2 u ($2/100000):(1./STATS_records) smooth cumulative notitle w l