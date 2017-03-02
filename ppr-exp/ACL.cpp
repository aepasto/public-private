#include <iostream>
#include <cassert>
#include <cmath>
#include <queue>
#include <vector>

#include "ACL.h"

using namespace std;

ACL::ACL() {
}

void ACL::run(UDynGraphMemEff* graph, int seed, double alpha, double epsilon,
    unordered_map<int, double>* scores) {

    // The algorithm uses the lazy random walks.
    // to have real-ppr(u,alpha) you need to use
    // lazy-ppr(u,(alpha)/(2.0 - alpha))

    this->alpha = alpha / (2.0 - alpha);
    //cerr<< "Using (Lazy)Alpha: " <<this->alpha <<" for (Real)Alpha:" << alpha;

    this->graph = graph;
    this->epsilon = epsilon;
    this->graph = graph;
    this->seed = seed;
    this->in_queue.clear();

    scores->clear();

    approximatePPR(scores);
}

void ACL::approximatePPR(unordered_map<int, double>* pProb) {

    vector<int> nodes;
    graph->nodes(&nodes);

    unordered_map<int, double> rProb;

    rProb[seed] = 1.0;

    NodeProbQueue* pQueue = new NodeProbQueue();

    pQueue->push(seed);

    while (!pQueue->empty()) {
        int uNode = pQueue->front();
        pQueue->pop();

        in_queue.erase(uNode);

        pushNode(pProb, &rProb, uNode, pQueue);
    }

    delete pQueue;
}

void ACL::pushNode(unordered_map<int, double> *p, unordered_map<int, double> *r,
    int u, NodeProbQueue* pQueue) {

    if (p->find(u) == p->end()) {
        p->insert(make_pair(u, 0.0));
    }
    if (r->find(u) == r->end()) {
        r->insert(make_pair(u, 0.0));
    }

    double pU = p->at(u);
    double rU = r->at(u);
    p->at(u) = pU + alpha * rU;
    r->at(u) = (1 - alpha) * rU / 2.0;

    updateQueue(pQueue, u, r->at(u));

    int degreeU = graph->degree(u);

    vector<int> neighbors;
    graph->neighbors(u, &neighbors);

    for (const auto& v : neighbors) {

        if (r->find(v) == r->end()) {
            r->insert(make_pair(v, 0.0));
        }

        r->at(v) += (1 - alpha) * rU / (2.0 * degreeU);

        updateQueue(pQueue, v, r->at(v));
    }
}

void ACL::updateQueue(NodeProbQueue* pQueue, int node, double prob) {
    int degNode = graph->degree(node);

    if ((prob / degNode) > epsilon && in_queue.find(node) == in_queue.end()) {
        pQueue->push(node);
        in_queue.insert(node);
    }
}

ACL::~ACL() {
}
