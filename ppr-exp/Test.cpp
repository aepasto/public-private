//============================================================================
// Name        : DensestSubgraph.cpp
// Author      : Alessandro Epasto
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================

#include "UDynGraphMemEff.h"
#include "GraphUtils.h"
#include "ACL.h"
#include "TimeStats.h"

#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

int main() {

	TimeStats time_stats;

	UDynGraphMemEff g;
	import_graph("/Users/aepasto/Desktop/private-star/expts/ppr-exp/test.txt",
			&g);
	vector<pair<int, int> > edges;
	g.edges(&edges);
	for (const auto & edge : edges) {
		cout << edge.first << " " << edge.second << endl;
	}

	ACL acl;
	unordered_map<int, double> scores;

	time_stats.begin_op();
	for(int i = 0; i<100; i++){
		acl.run(&g, 9, 0.10, 0.001, &scores);
	}
	time_stats.end_op();

	cout<< time_stats.stats()[0].micros <<endl;


	for (const auto & s_pair : scores) {
		cout << s_pair.first << " " << s_pair.second << endl;
	}

	// Test private

	vector<pair<int,vector<int>>> private_stars;
	import_private_graphs("/Users/aepasto/Desktop/private-star/expts/ppr-exp/test_private.txt",
			&private_stars);

	cout << "SIZE:" <<private_stars.size() << endl;
	for (const auto& star: private_stars){
		int center = star.first;
		auto& leaves = star.second;
		for (const auto& leaf: leaves){
			cout << center << "->" << leaf << endl;
		}
		cout << endl;
	}

	return 0;
}
