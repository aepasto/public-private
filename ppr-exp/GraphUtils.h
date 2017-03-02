/*
 * GraphScheduler.h
 *
 *  Created on: Oct 22, 2014
 *      Author: aepasto
 */

#ifndef GRAPHUTILS_H_
#define GRAPHUTILS_H_

#include "UDynGraphMemEff.h"

using namespace std;

void import_graph(const string& file_name, UDynGraphMemEff* graph);

// Output vec of pairs of: center, vector of leaves
// Input: file path with
// Line 1: center
// Line 2: leaves (separater by blank)
// ....
// Line 2k-1: center
// Line 2k: leaves (separater by blank)
// ...
void import_private_graphs(const string& file_name,
		vector<pair<int, vector<int>>>* vec);

#endif /* GRAPHUTILS_H_ */
