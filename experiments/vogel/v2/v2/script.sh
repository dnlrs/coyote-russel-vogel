#!/bin/bash

gcc -Wall -o vogel_v2.out main.c -lm

for i in $(ls tests/ | grep "_1_"); do
    p=`expr index $i ".txt"`;
    let "p-=1";
    #echo "$i: $p -> ${i:0:$p}";
    ./vogel_v2.out tests/$i >> res/output_${i:0:$p}.csv  2>>debug/err_${i:0:$p}.csv;
done;

for i in $(ls res/); do
    sort -t ";" -n -k3,3 res/$i | head -n 2 >> output.csv;
done;
