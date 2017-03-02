#include <iostream>
#include <cassert>
#include <cmath>
#include <queue>
#include <vector>

#include "Heuristics.h"

using namespace std;

Heuristics::Heuristics() {
}

void Heuristics::run(int seed, double alpha,
    vector<unordered_map<int, double>>& public_scores_neighbors,
    unordered_map<int, double>* scores) {

    double alpha_inv_degree = (1.0 - alpha) / public_scores_neighbors.size();
    for (const auto & pub_scores : public_scores_neighbors) {
        for (const auto & score : pub_scores) {
            if (scores->find(score.first) == scores->end()) {
                scores->insert(make_pair(score.first, 0));
            }
            scores->at(score.first) = scores->at(score.first)
                    + alpha_inv_degree * score.second;
        }
    }
    if (scores->find(seed) == scores->end()) {
        scores->insert(make_pair(seed, 0));
    }
    scores->at(seed) = scores->at(seed) + alpha;
}

Heuristics::~Heuristics() {
}
