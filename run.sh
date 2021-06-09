#!/bin/sh

make clean
make
# ./lrc_sim > log_file/test.log
./lrc_sim > log_file/test1.log
cp tests/sim-rand-write_lat.1.log tests/sim-rand-write_lat.csv
cd tests && ./plot.py
# ./lrc_sim