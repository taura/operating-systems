#!/bin/sh

# a.out を n 個並列に走らせたあと，
# それらの出力を全部まとめて表示する

n=10

# n個並列に走らせる
for i in $(seq 0 $((${n} - 1))) ; do
    ./a.out > x.${i} & 
done
wait

# 出力を表示
for i in $(seq 0 $((${n} - 1))) ; do
    cat x.${i}
done

