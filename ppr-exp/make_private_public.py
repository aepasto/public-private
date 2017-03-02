#! /usr/bin/env python

import argparse
import logging
import math
import random
from collections import defaultdict

def read_graph_nodes(filename):
  pos = 0
  nodes = set()
  for line in open(filename):
    line = line.strip()
    if line[0] == '#':
      continue
    line = line.split('\t')
    pair = (int(line[0].strip()),int(line[1].strip()))
    if pair[0] == pair[1]:
      continue

    nodes.add(pair[0])
    nodes.add(pair[1])

    if pos %1000000 == 0:
      print pos/1000000,"M"
    pos+=1

  return nodes
    
#    pair_rev = (pair[1], pair[0])
#    edges.append(pair)
#    edges.append(pair_rev)
  
  
def main():
  print "ASSUMES EDGES WRITTEN ONLY ONCE X,Y => not Y,X!!!"
  print "MAKES GRAPHS UNDIRECTED!!!"
  
  logging.basicConfig(format='[%(levelname)s] %(message)s',level=logging.DEBUG)
  parser = argparse.ArgumentParser(description='Accuracy of the rankings')
  parser.add_argument('-f', '--file', action='store', dest='filename', required=True)
  parser.add_argument('-o', '--out_public', action='store', dest='out_public', required=True)
  parser.add_argument('-s', '--out_private_stars', action='store', dest='out_private_stars', required=True)
  parser.add_argument('-k', '--k', action='store', dest='centers', required=True)

  
  conf = parser.parse_args()

  nodes = read_graph_nodes(conf.filename)
  centers = set(random.sample(nodes, int(conf.centers)))

  # Write public
  out_pub = open(conf.out_public, "w")
  neighbors_centers = defaultdict(list)

  pos = 0 
  for line in open(conf.filename):
    line = line.strip()
    if line[0] == '#':
      continue
    line = line.split('\t')
    x,y = (int(line[0].strip()),int(line[1].strip()))
    if x == y:
      continue
    if not x in centers and not y in centers:
      out_pub.write(str(x)+" "+str(y)+"\n")
      out_pub.write(str(y)+" "+str(x)+"\n")
    if x in centers:
      neighbors_centers[x].append(y)

    if pos %1000000 == 0:
      print pos/1000000,"M"
      
    pos+=1
  # Write private
  out_priv = open(conf.out_private_stars, "w")
  for x, neighbors in neighbors_centers.items():
    out_priv.write(str(x)+"\n")
    out_str = ""
    for n in neighbors:
      out_str += str(n)+" "
    out_priv.write(out_str.strip()+"\n")

if __name__ == '__main__':
    main()
