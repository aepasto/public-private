#!/usr/bin/env python

import sys, random, time

f = open(sys.argv[1], 'rU')

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

print >> sys.stderr, 'Extra Node:', L[-1]

precomputation = time.time()
cost = 0
C = []
I = [None] * len(L)
for n in L[:-1]:
    if I[n] == None:
        C.append([n])
        I[n] = len(C) - 1
        
        for x in V[n]:
            if I[x] == None and x != L[-1]:
                C[-1].append(x)
                I[x] = len(C) - 1
precomputation = time.time() - precomputation
                

#print C
#print V
print >> sys.stderr, '# nodes', len(L)
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


VtoC = [-1] * len(L)
for i in range(len(C)):
    for j in C[i]:
        VtoC[j] = i

cost = costC(V, C, I, L)
#print C

opt = len(L)

for i in range(len(C)):
    C[i] = set(C[i])

update = time.time()
addcost = len(V[L[-1]])

for y in V[L[-1]]:
    i = VtoC[y]
    c = len(V[L[-1]]) + len(C[i])
    c -= 2 * len(C[i] & V[L[-1]])

    if c < opt:
        opt = c

update =time.time() - update

print 'C = cost on the graph minus u (computed with acn) =', cost
print 'Cu = cost on the full instance (G + G_u) =', cost + opt
print '(Cu - C) / C =', (1.0 * opt) / cost

print 'tp = Time to compute C =', precomputation
print 'tu = Time to compute the clustering of G + G_u (given that G was already computed) =', update
print 'tu / tp =', update / precomputation




