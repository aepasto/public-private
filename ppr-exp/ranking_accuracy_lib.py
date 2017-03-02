
import math
from scipy import stats
import numpy 
import scipy 
from scipy.stats import pearsonr

def confidence(data, conf=0.95):
    a = 1.0*numpy.array(data)
    n = len(a)
    m, se = numpy.mean(a), scipy.stats.sem(a)
    h = se * scipy.stats.t._ppf((1+conf)/2., n-1)
    return m, h

def rmse(rankings):
  error_vec = []
  for ranking in rankings:
    error_sum = sum([(x-y)**2 for x,y in ranking])
    if len(ranking) <> 0:
      error_vec.append(math.sqrt(error_sum/len(ranking)))
  return error_vec

#too much weight to small values that are the most noisy
def mape(rankings):
  error_vec = []
  for ranking in rankings:
    error_sum = 0
    error_num = 0
    for x,y in ranking:
      if (x <> 0):
        error_sum += abs(x-y)/x
        error_num += 1
    if len(ranking) <> 0:
      error_vec.append(error_sum/error_num)
  return error_vec

def cosine_sim(rankings):
  sim_vec = []
  for ranking in rankings:
    inner_prod = sum([x*y for x,y in ranking])
    sum_sq_x = math.sqrt(sum([x**2 for x,y in ranking]))
    sum_sq_y = math.sqrt(sum([y**2 for x,y in ranking]))
    if len(ranking) <> 0:
      sim_vec.append(inner_prod/(sum_sq_x*sum_sq_y))
  return sim_vec

def pearson_p(rankings):
  r_vec = []
  p_vec = []
  for ranking in rankings:
    x_array = scipy.array([x for x,y in ranking])
    y_array = scipy.array([y for x,y in ranking])
    if len(x_array) < 3:
      continue
    r, p_value = pearsonr(x_array, y_array)
    r_vec.append(r)
    p_vec.append(p_value)
  return r_vec, p_vec


# Preprocessing:
# 1) If x or y < epsilon put it to zero.
# 2) Ignore positions with both elements zero.
# 3) Normalize the vectors to sum to 1.
def preprocessing(rankings, epsilon):
  rankings_n = []
  for rank in rankings:
    ranking = sorted(rank, key=lambda x: x[0], reverse=True)
    significant_ranking = []
    sum_x = 0
    sum_y = 0
    for x, y in ranking:
      assert x <> 0 or y <> 0, "both x and y zero"
      x_n = x if x >= epsilon else 0
      y_n = y if y >= epsilon else 0
      if x_n == 0 and y_n == 0:
        continue
      significant_ranking.append((x_n,y_n))
      sum_x += x_n
      sum_y += y_n
    for i in range(len(significant_ranking)):
      pair = significant_ranking[i]
      significant_ranking[i] = (pair[0]/sum_x,pair[1]/sum_y)
    rankings_n.append(significant_ranking)
  return rankings_n


#-If max_len > 0 uses only element that are in the top max_len position in the first ranking 
def make_array_x_y(rank, max_len):
  if (max_len > 0):
    ranking = rank[:max_len]
  else:
    ranking = rank
  x_array = scipy.array([x for x,y in ranking])
  y_array = scipy.array([y for x,y in ranking])

  return x_array, y_array

def tau(rankings, max_len):
  tau_vec = []
  p_vec = []
  for ranking in rankings:
    x_array, y_array = make_array_x_y(ranking, max_len)
    if len(x_array) < 3: # not possible to defeine tau
      continue
    tau, p_value = scipy.stats.kendalltau(x_array,y_array)
    tau_vec.append(tau)
    p_vec.append(p_value)
  return tau_vec, p_vec


def at_least_k_long(rankings, k):
  ranking_n = []
  for ranking in rankings:
    if len(ranking) >= k and ranking[k-1][0] <> 0:
      ranking_n.append(ranking)
  return ranking_n

def run_tau(rankings, max_k, conf_interval=0.95):
  # use all the rankings
  tau_all, tau_all_p_vec = tau(rankings, 0)
  tau_all_avg, tau_all_err = confidence(tau_all, conf_interval)
  print "All:", tau_all_avg, tau_all_err

  # For the furth analysis use only rankings with at least max_len position
  # nonzero in the original ranking
  rankings = at_least_k_long(rankings, max_k)
    
  for t in range(10, max_k+10, 10):
    tau_vec, tau_p_vec = tau(rankings, t)
    if len(tau_vec) == 0:
        break
    tau_avg, tau_err = confidence(tau_vec, conf_interval)
    print t, tau_avg, tau_err

def run_prec(rankings, conf_interval=0.95):
  rmse_vec = rmse(rankings)
  rmse_avg, rmse_err = confidence(rmse_vec, conf_interval)
  cosine_vec = cosine_sim(rankings)
  cosine_avg, cosine_err = confidence(cosine_vec, conf_interval)
  pearson_vec, pearson_p_vec = pearson_p(rankings)
  pearson_avg, pearson_err = confidence(pearson_vec, conf_interval)

  print rmse_avg,rmse_err,cosine_avg,cosine_err,pearson_avg,pearson_err
