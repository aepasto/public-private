#!/usr/bin/env python

import sys, random, time

f = open(sys.argv[1], 'rU')

print 'ACN on the full graph'
V = []

for line in f:
    if line[0] == '#':
        continue
    if line[-1] == '\n':
        line = line[:-1]
    L = line.split()
    (f,t,s) = (int(L[0]), int(L[1]), int(L[2]))

    if s == 1:
        while len(V) <= max(f, t):
            V.append(set())

        V[f].add( t )
        V[t].add( f )


L = range(len(V))
random.shuffle(L)

precomputation = time.time()
cost = 0
C = []
I = [None] * len(L)
for n in L:
    if I[n] == None:
        C.append([n])
        I[n] = len(C) - 1
        
        for x in V[n]:
            if I[x] == None:
                C[-1].append(x)
                I[x] = len(C) - 1
precomputation = time.time() - precomputation
                

#print C
#print V
print >> sys.stderr, '# clusters:', len(C)
def costC(V, C, I, L):
    cost = 0
    for k in range(len(C)):
        c = C[k]
        #if k % 10000 == 0:
        #    print >> sys.stderr, k, '/', len(C)
        for i in range(len(c)):
            for j in range(i+1,len(c)):
                if c[j] not in V[c[i]]:
                    cost += 1

                
    for n in L[:-1]:
        if I[n] != None:
            for x in V[n]:
                if I[x] != None:
                    if n < x and I[n] != I[x]:
                        cost += 1
    return cost

                
cost = costC(V, C, I, L)
print 'cost:', cost
print 'time:', precomputation
