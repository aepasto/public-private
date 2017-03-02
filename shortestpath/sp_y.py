#!/usr/bin/env python

import sys, random, time, heapq

V = []

for line in open(sys.argv[1], 'rU'):
    if line[0] == '#':
        continue
    if line[-1] == '\n':
        line = line[:-1]
    L = line.split()
    (f,t) = (int(L[0]), int(L[1]))

    while len(V) <= max(f, t):
        V.append(set())

    V[f].add( t )
    V[t].add( f )


def sssp(V,s,avoid=None):
    '''Single Source Shortest Path'''
    D = [-1] * len(V)
    C = [-1] * len(V)
    Q = {0: {}}
    for v in s:
        if v != avoid:
            Q[0][v] = True
            C[v] = v
            D[v] = 0

    d = 0
    while d in Q:
        N = Q[d].keys()
        Q[d] = {}
        for x in N:
            for y in V[x]:
                if y != avoid:
                    if D[y] > D[x] + 1:
                        Q[D[y]].pop(y)
                        D[y] = -1
                    if D[y] == -1:
                        D[y] = D[x] + 1
                        if D[y] not in Q:
                            Q[D[y]] = {}
                        Q[D[y]][y] = True
                        C[y] = C[x]
        d += 1

    return (D, C)

(S, x) = sssp(V, [0])
count = 0
for i in range(len(S)):
    if S[i] == -1:
        count += 1
if count > 0:
    print 'The graph is not connected!'

        

extra = random.randint(0, len(V) - 1)
print 'Extra Node:', extra

L = range(len(V))
L.remove(extra)

S = []
l = 1
multiplicative_value = int(sys.argv[2])
while l / multiplicative_value < len(V) - 1:
    j = 1
    while j / multiplicative_value < len(V) - 1:
        random.shuffle(L)
        S.append(L[:l])
        j *= multiplicative_value
    l *= multiplicative_value

#print 'Beacon sets:', S

precomptotaltime = 0.0
precompnumber = 0
DS = []
for n in range(len(V)):
    DS.append([])
for i in range(len(S)):
    s = S[i]
    print 'Working on a beacon set of size', len(s), '(', i, '/', len(S), ')'
    t = time.time()
    (X, C) = sssp(V, s, avoid = extra)
    precomptotaltime += time.time() - t
    precompnumber += 1

    for n in range(len(V)):
        if n != extra:
            DS[n].append( (C[n], X[n]) )
sizeSketch = 0
for n in range(len(V)):
    sizeSketch = sizeSketch + len(DS[n])
print 'Size of the sketches', sizeSketch

#print S
#print DS

def estDist(DS, v,w):
    d = -1
    for i in range(len(DS[0])):
        if DS[v][i] != None and DS[w][i] != None:
            if DS[v][i][0] == DS[w][i][0] and DS[v][i][0] != -1:
                if d == -1 or DS[v][i][1] + DS[w][i][1] < d:
                    d = DS[v][i][1] + DS[w][i][1]
    return d

precompavgtime = precomptotaltime / precompnumber

print 'SSSP avg precomputation time:', precompavgtime

while True:
    second = random.randint(0, len(V) - 1)
    if second != extra:
        break


print 'Second node:', second

update = time.time()
mind = -1
for x in V[extra]:
    ed = estDist(DS, x, second)
    if mind == -1 or ed + 1 < mind:
        mind = ed + 1
# approximate distance between extra and second is mind
print 'Approximate distance:', mind
update = time.time() - update

timeSP = time.time()
U = []
U.append(extra)
(K,J) = sssp(V,U,avoid = None)
print 'Real distance:', K[second]
recomputing = time.time() - timeSP

print 'Distance ratio:', mind / K[second]

print 'Computation time of SSSP:', recomputing
print 'Computation time of an approximate SSSP:', update
print 'Computation time as a fraction of avg precomputation:', update / precompavgtime
