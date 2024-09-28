#!/bin/sh -x

# 数をかえる, taskset -c 0 を付けるなどしてみよ

n=30

for i in $(seq 0 $((${n} - 1))) ; do 
    if [ ${i} = 0 ]; then 
#	taskset -c 0 ./a.out 1.0e-4 10000 0.02 0.0 > vr.${i} &
	taskset -c 0 ./a.out 1.0e-4 10000 0.0001 0.001 > vr.${i} &
    else
	taskset -c 0 ./a.out 1.0e-4 10000 0.02 0.0 > vr.${i} &
    fi
done
wait

files=""
for i in $(seq 0 $((${n} - 1))) ; do 
    if [ ${i} -gt 0 ]; then
	files="${files}, "
    fi
    files="${files}\"vr.${i}\""
done

# グラフを同時に可視化.
gnuplot -e "set xtics rotate by -30" -e "plot ${files}" -e 'pause -1'
