#!/bin/bash
gcc -Wall -o russel_v3.out main.c -lm

if [ $? -ne 0 ]; then
    exit;
fi

for i in $(ls hardtest/); do
    p=`expr index $i ".txt"`;
    let "p-=1";
    ./russel_v3.out hardtest/$i >> res/output_${i:0:$p}.csv  2>>debug/err_${i:0:$p}.csv;
done;



for i in $(ls res/); do
    sort -t ";" -n -k3,3 -k2,2 res/$i | head -n 1 >> outputHardScript.csv;
done;
