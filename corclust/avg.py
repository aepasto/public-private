#!/usr/bin/env python

import sys

L = []
sum = 0.0
n = 0
for l in sys.stdin.readlines():
    if l[-1] == '\n':
        l = l[:-1]
    L.append(float(l))
    sum += L[-1]
    n += 1

avg = sum / n

std = 0.0
for x in L:
    std += (x - avg)**2
std /= n
print avg, std**0.5

