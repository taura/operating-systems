#!/bin/sh

./a.out > x.0 & 
./a.out > x.1 & 
./a.out > x.2 & 
./a.out > x.3 &
./a.out > x.4 &
wait

cat x.0 x.1 x.2 x.3 x.4
