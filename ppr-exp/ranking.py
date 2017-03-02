#! /usr/bin/env python

import argparse
import logging
import math
from scipy import stats
import numpy 
import scipy 
from scipy.stats import pearsonr
import ranking_accuracy_lib

"""
  Use as file in input

  0.5,0.5
  0.2,0.2
  0.1,0.1
  #
  0.15,0.145
  0.12,0.10
  0.11,0.11
  #
  0.5,0.51
  0.2,0.19
  0.1,0.1
  #

  where # ends each ranking
  and each ranking is a pair of
  prob_in_rank_correct_of_node_i, prob_in_rank_approx_of_node_i
  ...
"""

def read_ranking(filename):
  rankings = []
  elements = 0
  ranking = []
  for line in open(filename):
    #end of ranking
    if line.startswith('#') and elements <> 0:
      rankings.append(ranking)
      elements = 0
      ranking = []
      continue
    elif line.startswith('#'):
      continue
    line = line.split(',')
    pair = (float(line[0].strip()),float(line[1].strip()))
    ranking.append(pair)
    elements += 1
  return rankings

  
def main():

  
  logging.basicConfig(format='[%(levelname)s] %(message)s',level=logging.DEBUG)
  parser = argparse.ArgumentParser(description='Accuracy of the rankings')
  parser.add_argument('-f', '--file', action='store', dest='filename', required=True)
  
  conf = parser.parse_args()

  rankings = read_ranking(conf.filename)

  # Preprocessing:
  # 1) If x or y < epsilon put it to zero.
  # 2) Ignore positions with both elements zero.
  # 3) SORT THE RANKING by x. It is necessary to call preprocessing before tau
  # 3) normalize the vectors to sum to 1.
  rankings = ranking_accuracy_lib.preprocessing(rankings, 0.0000001)

  ranking_accuracy_lib.run_prec(rankings, 0.95)
  
  ranking_accuracy_lib.run_tau(rankings, 50, 0.95)
  
if __name__ == '__main__':
    main()
