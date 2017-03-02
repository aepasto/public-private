#ifndef ACL_H
#define ACL_H

#include <vector>
#include <queue>
#include <unordered_map>

#include "UDynGraphMemEff.h"

#include <unordered_map>
#include <unordered_set>

using namespace std;
typedef queue<int> NodeProbQueue;

class ACL {
private:

	double alpha = 0;
	double epsilon = 0;
	int seed = 0;
	UDynGraphMemEff* graph = NULL;
	unordered_set<int> in_queue;


	void updateQueue(NodeProbQueue* pQueue, int node, double prob);
	void approximatePPR(unordered_map<int, double>* p);
	void pushNode(unordered_map<int, double>* p, unordered_map<int, double>* r, int u, NodeProbQueue* pQueue);

public:
	ACL();
	virtual ~ACL();
	void run(UDynGraphMemEff* graph, int seed, double alpha, double epsilon,
			unordered_map<int, double>* score);
};

#endif
