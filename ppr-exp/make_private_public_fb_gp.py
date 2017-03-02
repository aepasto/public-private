#! /usr/bin/env python

import argparse
import logging
import math
import random
import glob
from collections import defaultdict


def read_circles(directory, prob_priv):
  files = glob.glob(directory+"/*.circles")
  private_edges = defaultdict(set)
  
  for f in files:
    node = int(f.split('/')[-1].split('.')[0].strip())
    f_in = open(f)
    for line in f_in:
      circle = [int(x) for x in line.strip().split('\t')[1:]]
      if random.random() <= float(prob_priv):
        for x in circle:
          private_edges[node].add(x)

  nodes = private_edges.keys()
  centers = random.sample(nodes, min(100,len(nodes)))

  private_edges_out = defaultdict(set)
  for x in centers:
    private_edges_out[x] = private_edges[x]
    
  return private_edges_out

def read_graph(filename):

  DICT = {}
  
  pos = 0
  edges = set()
  for line in open(filename):
    line = line.strip()
    if line[0] == '#':
      continue
    line = line.split(' ')
    pair = (int(line[0].strip()),int(line[1].strip()))
    if pair[0] == pair[1]:
      continue
    if pair[0] not in DICT:
      DICT[pair[0]] = len(DICT)
    if pair[1] not in DICT:
      DICT[pair[1]] = len(DICT)
    
    edges.add((pair[0],pair[1]))
    edges.add((pair[1],pair[0]))

  return edges, DICT
              
  
def main():
  print "MAKES GRAPHS UNDIRECTED!!!"
  
  logging.basicConfig(format='[%(levelname)s] %(message)s',level=logging.DEBUG)
  parser = argparse.ArgumentParser(description='Accuracy of the rankings')
  parser.add_argument('-f', '--file', action='store', dest='filename', required=True)
  parser.add_argument('-d', '--dir_circles', action='store', dest='dir_circles', required=True)
  parser.add_argument('-o', '--out_public', action='store', dest='out_public', required=True)
  parser.add_argument('-s', '--out_private_stars', action='store', dest='out_private_stars', required=True)
  parser.add_argument('-p', '--prob_circle_private', action='store', dest='prob_circle_private', required=True)

  
  conf = parser.parse_args()

  private_edges = read_circles(conf.dir_circles, conf.prob_circle_private)
  edges, DICT = read_graph(conf.filename)
  

  # Write public
  out_pub = open(conf.out_public, "w")
  neighbors_centers = defaultdict(list)

  for x,y in edges:
    if (not x in private_edges or not y in private_edges[x]) and (not y in private_edges or not x in private_edges[y]):
      out_pub.write(str(DICT[x])+" "+str(DICT[y])+"\n")
      out_pub.write(str(DICT[y])+" "+str(DICT[x])+"\n")


  # Write private
  out_priv = open(conf.out_private_stars, "w")
  for x, neighbors in private_edges.items():
    out_priv.write(str(DICT[x])+"\n")
    out_str = ""
    for n in neighbors:
      out_str += str(DICT[n])+" "
    out_priv.write(out_str.strip()+"\n")

if __name__ == '__main__':
    main()
