#!/bin/sh 
 gcc -Wall -g -o russel.out main.c -lm
 
 for i in $(ls tests/); do
    ./russel.out tests/$i > output.csv tail -n 2 output.csv;
done;
