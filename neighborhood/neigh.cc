#include <assert.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "ngraph.hpp"

using namespace NGraph;
using namespace std;

// Utilities
// Returns current time.
string Now() {
  time_t t = time(0);
  struct tm* now = localtime(&t);
  char buf[80];
  strftime(buf, sizeof(buf), "%H:%M:%S", now);
  return buf;
}

// For fatal errors.
void Die(const string& s) {
  cerr << "FATAL: " << s << endl;
  exit(1);
}

// Bottom k data structure.
class Bottomk {
 public:
  Bottomk() {
    SetK(0);
  }

  Bottomk(int k) {
    SetK(k);
  }

  void SetK(int k) {
    k_ = k;
  }
 
  int Size() const {
    return v_.size();
  }

  double Max() const {
    return v_.front();
  }

  void Add(double x) {
    if (s_.find(x) != s_.end()) {
      return;
    }
    v_.push_back(x);
    s_.insert(x);
    push_heap(v_.begin(), v_.end());
    if (v_.size() > k_) {
      pop_heap(v_.begin(), v_.end());
      double y = v_.back();
      v_.pop_back();
      s_.erase(y);
    } 
  }

  void Add(const Bottomk& in) {
    for (int i = 0; i < in.Size(); i++) {
      Add(in.v_[i]);
    }
  }

  bool operator==(const Bottomk& in) {
    if (s_.size() != in.s_.size()) {
      return false;
    }
    for (const auto& x : s_) {
      if (in.s_.find(x) == in.s_.end()) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Bottomk& in) {
    return !(*this == in);
  }

  void Clear() {
    v_.clear();
    s_.clear();
  }

  void Print(const string& s) const {
    printf("%s[k=%d]:", s.c_str(), k_);
    for (int i = 0; i < v_.size(); i++) {
      printf(" %f ", v_[i]);
    }
    printf("\n");
    printf("\t set:");
    for (const auto& x : s_) {
      printf(" %f ", x);
    }
    printf("\n");
  }

 private:
  int k_;
  vector<double> v_;
  set<double> s_;
};

void TestBottomk() {
  Bottomk bot(3);
  bot.Add(10.0);
  bot.Add(30.0);
  bot.Add(20.0);
  bot.Print("initbot");
  bot.Add(40.0);
  bot.Print("bot after adding 40");
  bot.Add(5.0);
  bot.Print("bot after adding 5.0");
  Bottomk tob(3);
  tob.Add(bot);
  tob.Print("tob");
  printf("equal? %d\n", bot == tob);
  printf("not equal? %d\n", bot != tob);
  tob.Add(1.0);
  printf("equal? %d\n", bot == tob);
  printf("not equal? %d\n", bot != tob);
  exit(1);
}

// Parameter class.
class Params {
 public:
  Params() {
    debug_ = false;
    epsilon_ = 0.1;
    filename_ = "wiki-Vote.graph";
    seed_ = 1;
    star_ = true;
    undirected_ = false;
    trials_ = 100;
  }

  Params(int argc, char** argv) {
    Parse(argc, argv);
  }

  void Parse(int argc, char** argv) {
    cout << "clp=";
    for (int i = 0; i < argc; i++) {
      cout << argv[i] << " ";
    }
    cout << endl;
    static struct option long_options[] = {
      {"clique", no_argument, 0, 'c'},
      {"debug", no_argument, 0, 'd'},
      {"epsilon", required_argument, 0, 'e'},
      {"file", required_argument, 0, 'f'},
      {"seed", required_argument, 0, 's'},
      {"trials", required_argument, 0, 't'},
    };
    while (1) {
      int option_index = 0;
      switch (getopt_long(argc, argv, "cde:f:s:t:", long_options,
                          &option_index)) {
        case -1:
          return;
        case 'c':
          star_ = false;
          break;
        case 'd':
          debug_ = true;
          break;
        case 'e':
          epsilon_ = atof(optarg);
          break;
        case 'f':
          filename_ = optarg;
          break;
        case 's':
          seed_ = atol(optarg);
          break;
        case 't':
          trials_ = atol(optarg);
          break;
        case 'u':
          undirected_ = true;
          break;
        default:
          Die("getopt received junk");
      }
    }
  }

  void Print() const {
    cerr << "# star=" << star_ << endl;
    cerr << "# debug=" << debug_ << endl;
    cerr << "# epsilon=" << epsilon_ << endl;
    cerr << "# filename=" << filename_ << endl;
    cerr << "# seed=" << seed_ << endl;
    cerr << "# trials=" << trials_ << endl;
    cerr << "# undirected=" << undirected_ << endl;
  }

  double epsilon_;
  long seed_;
  string filename_;
  int trials_;
  bool star_;
  bool debug_;
  bool undirected_;
};

class Sketch {
 public:
  Sketch() {
    SetEpsilon(0.1);
  }

  Sketch(double epsilon) {
    SetEpsilon(epsilon);
  }

  void SetEpsilon(double epsilon) {
    epsilon_ = epsilon;
  }

  double SketchGraph(const Graph& G) {
    clock_t time_begin = clock();
    int n = G.num_nodes();
    s_.clear();
    s_.resize(n);
    r_.resize(n);
    news_.resize(n);
    int k = static_cast<int>((2 + c_) * log(n) / (epsilon_ * epsilon_));
    cerr << "k=" << k << endl;
    for (int i = 0; i < n; i++) {
      r_[i] = drand48();
      s_[i].SetK(k);
      s_[i].Add(r_[i]);
      news_[i].SetK(k);
      news_[i].Clear();
    }
    cerr << "created data structure\n"; 
    for (int iter = 0; ; iter++) {
      cerr << "iteration upto diameter " << iter << endl;  
      for (Graph::const_iterator pi = G.begin(); pi != G.end(); pi++) {
        unsigned int i = G.node(pi);
        news_[i].Clear();
        news_[i].Add(s_[i]);
        assert(i >= 0 && i < n);
        const Graph::vertex_set& out = G.out_neighbors(i);
        for (Graph::vertex_set::const_iterator j = out.begin(); j != out.end();
             j++) {
          assert(*j >= 0 && *j < n);
          news_[i].Add(s_[*j]);
        }
      }
      // news[0].Print("data");
      bool done = true;
      for (int i = 0; done && i < n; i++) {
        if (s_[i] != news_[i]) {
          cerr << "not done at index " << i << endl;
          done = false;
        }
      }  
      if (done) {
        break;
      }
      for (int i = 0; i < n; i++) {
        s_[i].Clear();
        s_[i].Add(news_[i]);
      }
    }
    clock_t time_end = clock();
    return (time_end - time_begin)/(double)(CLOCKS_PER_SEC/1000);
  }

  double RecomputeForNewNode(const Graph& G, const vector<pair<int, int>> Eup,
                             const set<int>& out, int u) {
    clock_t time_begin = clock();
    if (r_.size() <= u) {
      assert(r_.size() == u);
      r_.resize(u+1);
      s_.resize(u+1);
      news_.resize(u+1);
      r_[u] = drand48();
      s_[u].Add(r_[u]);
    }
    for (const auto& z : out) {  
      for (int i = 0; i < Eup.size(); i++) {
        int w = Eup[i].first, v = Eup[i].second;
        if (w == z) {
          s_[z].Add(r_[v]);
          s_[z].Add(s_[v]);
        }
      }
    }
    for (int i = 0; i < Eup.size(); i++) {
      int w = Eup[i].first, v = Eup[i].second;
      if (w == u) {
        s_[u].Add(r_[v]);
        s_[u].Add(s_[v]);
      }
    }
    clock_t time_end = clock();
    return (time_end - time_begin)/(double)(CLOCKS_PER_SEC/1000);
  }

 private:
  const double c_ = 0.1;
  double epsilon_;
  vector<Bottomk> s_;
  vector<double> r_;
  vector<Bottomk> news_;
};

void ReadGraph(const string& filename, Graph* G) {
  ifstream fp;
  string line = "";
  fp.open(filename.c_str());
  if (fp.fail()) {
    Die(filename + ": cannot open file");
  }
  do {
    if (line.size() == 0 || line[0] == '#') {
      cout << "Skipping: " << line << endl;
      continue;
    }
    istringstream iss(line);
    int u, v;
    iss >> u >> v;
    G->insert_edge(u, v);
  } while(getline(fp, line));
  fp.close();
}

int UpdateGraph(bool star, Graph* G, vector<pair<int, int>>* new_edges) {
  new_edges->clear();
  int n = G->num_nodes();
  cerr << "updategraph n=" << n << endl;
  vector<int> neigh;
  int r;
  while (neigh.size() == 0) {
    r = int(drand48() * n);
    cerr << "Node=" << r << endl;
    const Graph::vertex_set& out = G->out_neighbors(r);
    for (Graph::vertex_set::const_iterator j = out.begin(); j != out.end();
         j++) {
      neigh.push_back(*j);
    }
    cerr << "Neighborhood size=" << neigh.size() << endl;
  }
  if (star) {
    for (int j = 0; j < neigh.size(); j++) {
      new_edges->push_back(pair<int, int>(n, neigh[j]));
      if (G->includes_edge(neigh[j], r)) {
        new_edges->push_back(pair<int, int>(n, neigh[j]));
      }
    }
  } else {
    for (int j1 = 0; j1 < neigh.size(); j1++) {
      new_edges->push_back(pair<int, int>(n, neigh[j1]));
      new_edges->push_back(pair<int, int>(neigh[j1], n));
      for (int j2 = 0; j2 < neigh.size(); j2++) {
        if (j1 != j2) {
          new_edges->push_back(pair<int, int>(neigh[j1], neigh[j2]));
        }
      }
    }
  }
  for (int i = 0; i < new_edges->size(); i++) {
    G->insert_edge((*new_edges)[i].first, (*new_edges)[i].second);
  }
  cerr << "Edges added=" << new_edges->size() << endl;
  return n;
}

void FilterEdges(const vector<pair<int, int>> Eu, int u,
                 vector<pair<int, int>>* Eup, set<int>* out) {
  Eup->clear();
  out->clear();
  for (int i = 0; i < Eu.size(); i++) {
    int w = Eu[i].first, v = Eu[i].second;
    if (w == u) {
      out->insert(v);
    }
  }
  for (int i = 0; i < Eu.size(); i++) {
    int w = Eu[i].first, v = Eu[i].second;
    if (v == u || (w != u && out->find(v) != out->end())) {
      continue;
    }
    Eup->push_back(pair<int, int>(w, v));
  }
  cerr << "Out size=" << out->size() << endl;
  cerr << "Filtered edges=" << Eup->size() << endl;
}

int main(int argc, char **argv) {
  // TestBottomk();
  Params params;
  params.Parse(argc, argv);
  params.Print();
  srand48(params.seed_);
  Graph G;
  if (params.undirected_) {
    G.set_undirected();
  }
  cerr << "Reading graph " << Now() << endl;
  ReadGraph(params.filename_, &G);
  cerr << "Done " << Now() << endl;
  cerr << "# n=" << G.num_nodes() << endl;
  cerr << "# m=" << G.num_edges() << endl;
  Sketch S(params.epsilon_);
  cerr << "Sketching " << Now() << endl;
  double ntime = S.SketchGraph(G);
  cerr << "time for sketching=" << ntime << endl;

  double utime = 0.0;
  for (int i = 0; i < params.trials_; i++) {
    Graph newG = G;
    vector<pair<int, int>> Eu;
    int u = UpdateGraph(params.star_, &newG, &Eu);
    cerr << "u=" << u << endl;
    vector<pair<int, int>> Eup;
    set<int> out;
    FilterEdges(Eu, u, &Eup, &out);
    utime += S.RecomputeForNewNode(G, Eup, out, u);
  }
  utime /= params.trials_;
  cerr << "time for update=" << utime << endl;
  cout << "tu / tp = " << utime / ntime << endl;
}
