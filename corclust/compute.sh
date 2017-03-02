#!/bin/bash

for file in soc-sign-Slashdot090221.txt soc-sign-epinions.txt; do
    (for ((i=0;i<100;i++)); do ./corclust.py $file 2> /dev/null | cut -d' ' -f3; done) > $file.res
    sort -g $file.res > $file.out
done
