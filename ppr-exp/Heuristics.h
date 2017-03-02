#ifndef HEURISTICS_H_
#define HEURISTICS_H_

#include <vector>
#include <queue>
#include <unordered_map>


using namespace std;

class Heuristics {
private:

public:
    Heuristics();
    virtual ~Heuristics();

    void run(int seed, double alpha,
        vector<unordered_map<int, double>>& public_scores_neighbors,
        unordered_map<int, double>* scores);
};

#endif /* HEURISTICS_H_ */
