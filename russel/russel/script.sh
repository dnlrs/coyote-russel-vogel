#!/bin/bash
gcc -Wall -o russel.out main.c -lm

for i in $(ls tests/ | grep "_1_"); do
    p=`expr index $i ".txt"`;
    let "p-=1";
    ./russel.out tests/$i >> res/output_${i:0:$p}.csv  2>>debug/err_${i:0:$p}.csv;
done;



for i in $(ls res/); do
    sort -t ";" -n -k3,3 -k2,2 res/$i | head -n 1 >> output.csv;
done;
