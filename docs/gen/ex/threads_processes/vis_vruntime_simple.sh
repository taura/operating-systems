#!/bin/sh

# 数をかえる, taskset -c 0 を付けたりとったりしてみよ
# 1.0e-4 10000 0.1 0.05  
taskset -c 0 ./a.out > vr.0 &
taskset -c 0 ./a.out > vr.1 &
taskset -c 0 ./a.out > vr.2 &
taskset -c 0 ./a.out > vr.3 &
wait

# グラフを同時に可視化.

gnuplot -e 'plot "vr.0", "vr.1", "vr.2", "vr.3"' -e 'pause -1'
