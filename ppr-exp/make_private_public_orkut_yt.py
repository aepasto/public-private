#! /usr/bin/env python

import argparse
import logging
import math
import random
import glob
from collections import defaultdict


def read_circles(file):
  f_in = open(file)

  node_circle_ids = defaultdict(list)

  ids = 0
  
  for line in f_in:
    ids += 1
    circle = [int(x) for x in line.strip().split('\t')]
    for x in circle:
      node_circle_ids[x].append(ids)

  node_circle_id = {}
  for x, y_set in node_circle_ids.items():
    t = random.sample(set(y_set), 1)
    node_circle_id[x] = t[0]
  return node_circle_id


def centers(filename, node_circle_id):

  candidates = set()

  neighbors_circles = defaultdict(set)

  pos = 0 
  for line in open(filename):
    line = line.strip()
    if line[0] == '#':
      continue
    line = line.split('\t')
    x,y = (int(line[0].strip()),int(line[1].strip()))
    if x == y:
      continue
    if x in node_circle_id and y in node_circle_id:      
      neighbors_circles[x].add(node_circle_id[y])
      neighbors_circles[y].add(node_circle_id[x])
    if pos %1000000==0:
      print pos/1000000
      
    pos+=1
    
  for x, y_set in neighbors_circles.items():
    if len(y_set) >=2:
      candidates.add(x)

  centers = random.sample(candidates, 20)
  private_circles = defaultdict(list)
  for x in centers:
    pr = random.sample(neighbors_circles[x], int(max(1, 1.0*len(neighbors_circles[x])/2)))
    private_circles[x].extend(pr)
  
  return private_circles


def output_public(filename, private_circles, node_circle_id, outfile, out_private_stars):

  out = open(outfile, "w")

  private_edges = defaultdict(list)
  
  pos = 0 
  for line in open(filename):
    line = line.strip()
    if line[0] == '#':
      continue
    line = line.split('\t')
    x,y = (int(line[0].strip()),int(line[1].strip()))
    if x == y:
      continue
    if (not x in private_circles or not y in node_circle_id or not node_circle_id[y] in private_circles[x]) and (not y in private_circles or not x in node_circle_id or not node_circle_id[x] in private_circles[y]):      
      out.write(str(x)+" "+str(y)+"\n")
      out.write(str(y)+" "+str(x)+"\n")
    if x in private_circles and y in node_circle_id and node_circle_id[y] in private_circles[x]:
      private_edges[x].append(y)
      
    if pos %1000000==0:
      print pos/1000000
    pos+=1

  out_priv = open(out_private_stars, "w")
  for x, neighbors in private_edges.items():
    out_priv.write(str(x)+"\n")
    out_str = ""
    for n in neighbors:
      out_str += str(n)+" "
    out_priv.write(out_str.strip()+"\n")
  
##  print l 
##    
##    if random.random() <= float(prob_priv):
##     for x in circle:
##       private_edges[node].add(x)
##
##  nodes = private_edges.keys()
##  centers = random.sample(nodes, min(100,len(nodes)))
##
##  private_edges_out = defaultdict(set)
##  for x in centers:
##    private_edges_out[x] = private_edges[x]
##    
##  return private_edges_out
##
##def read_graph(filename):
##
##  DICT = {}
##  
##  pos = 0
##  edges = set()
##  for line in open(filename):
##    line = line.strip()
##    if line[0] == '#':
##      continue
##    line = line.split(' ')
##    pair = (int(line[0].strip()),int(line[1].strip()))
##    if pair[0] == pair[1]:
##      continue
##    if pair[0] not in DICT:
##      DICT[pair[0]] = len(DICT)
##    if pair[1] not in DICT:
##      DICT[pair[1]] = len(DICT)
##    
##    edges.add((pair[0],pair[1]))
##    edges.add((pair[1],pair[0]))
##
##  return edges, DICT
##              
##

    
def main():
  print "MAKES GRAPHS UNDIRECTED!!!"
  
  logging.basicConfig(format='[%(levelname)s] %(message)s',level=logging.DEBUG)
  parser = argparse.ArgumentParser(description='Accuracy of the rankings')
  parser.add_argument('-f', '--file', action='store', dest='filename', required=True)
  parser.add_argument('-c', '--circles', action='store', dest='circles', required=True)
  parser.add_argument('-o', '--out_public', action='store', dest='out_public', required=True)
  parser.add_argument('-s', '--out_private_stars', action='store', dest='out_private_stars', required=True)

  
  conf = parser.parse_args()

  node_circle_id = read_circles(conf.circles)
  print len(node_circle_id)
  private_circles = centers(conf.filename, node_circle_id)
  output_public(conf.filename, private_circles, node_circle_id, conf.out_public, conf.out_private_stars)

##  edges, DICT = read_graph(conf.filename)
##  
##
##  # Write public
##  out_pub = open(conf.out_public, "w")
##  neighbors_centers = defaultdict(list)
##
##  for x,y in edges:
##    if (not x in private_edges or not y in private_edges[x]) and (not y in private_edges or not x in private_edges[y]):
##      out_pub.write(str(DICT[x])+" "+str(DICT[y])+"\n")
##      out_pub.write(str(DICT[y])+" "+str(DICT[x])+"\n")
##
##
##  # Write private
##  out_priv = open(conf.out_private_stars, "w")
##  for x, neighbors in private_edges.items():
##    out_priv.write(str(DICT[x])+"\n")
##    out_str = ""
##    for n in neighbors:
##      out_str += str(DICT[n])+" "
##    out_priv.write(out_str.strip()+"\n")

if __name__ == '__main__':
    main()
