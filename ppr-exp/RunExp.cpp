#include "UDynGraphMemEff.h"
#include "GraphUtils.h"
#include "ACL.h"
#include "Heuristics.h"
#include "TimeStats.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cassert>

using namespace std;

void output_rankings(unordered_map<int, double>& correct,
    unordered_map<int, double>& approx, fstream& output) {
    unordered_set<int> nodes;

    for (const auto& pairs : correct) {
        nodes.insert(pairs.first);
    }
    for (const auto& pairs : approx) {
        nodes.insert(pairs.first);
    }

    for (const auto& node : nodes) {

        output << correct[node] << "," << approx[node] << endl;
    }
    output << "#" << endl;
}

int main(int argc, char** argv) {

    if (argc < 6) {
        cerr
                << "Use: public_graph_file; private_graphs_file; alpha; epsilon; output_file;";
        return 1;
    }

    string pub_graph_file = argv[1];
    string priv_graph_file = argv[2];
    double alpha = atof(argv[3]);
    double epsilon = atof(argv[4]);
    string output_file = argv[5];

    fstream output(output_file.c_str(), ios::out);

    TimeStats stats_corr;
    TimeStats stats_heur;
    ACL acl;
    Heuristics heur;
    UDynGraphMemEff graph;

    import_graph(pub_graph_file.c_str(), &graph);
    cout << "Imported Graph!" << endl;

    vector<pair<int, vector<int>>> private_stars;
    import_private_graphs(priv_graph_file, &private_stars);

    unordered_map<int, double> scores_pub;
    unordered_map<int, double> scores_priv_corr;
    unordered_map<int, double> scores_priv_heur;

    vector<int> in_public_vec;
    graph.nodes(&in_public_vec);
    unordered_set<int> in_public(in_public_vec.begin(), in_public_vec.end());

    int n_star = 0;

    for (const auto& star : private_stars) {
        const auto& center = star.first;
        const auto& leaves = star.second;

        cout << "Run:" << n_star++ << endl;

        // Precompute Public Graph Scores Restart on Neighbors
        vector<int> all_neighbors_v;
        graph.neighbors(center, &all_neighbors_v);
        unordered_set<int> all_neighbors(all_neighbors_v.begin(),
                all_neighbors_v.end());
        for (const auto& leaf : leaves) {
            all_neighbors.insert(leaf);
        }

        vector<unordered_map<int, double>> scores_pub_neighbor;

        for (const auto& neighbor : all_neighbors) {
            if (in_public.find(neighbor) != in_public.end()) {
                unordered_map<int, double> vec;
                acl.run(&graph, neighbor, alpha, epsilon, &vec);
                scores_pub_neighbor.push_back(vec);
            }
        }

        if (scores_pub_neighbor.size() == 0) {
            // No neighbor in public graph meaningless heuristics
            continue;
        }

        // Add Priv Edges
        for (const auto& leaf : leaves) {
            assert(graph.add_edge(center, leaf));
        }

        // Correct Private Score
        //First for the timer use same epsilon
        stats_corr.begin_op();
        acl.run(&graph, center, alpha, epsilon, &scores_priv_corr);
        stats_corr.end_op();

        //USES A SMALL EPSILON FOR GROUND TRUTH
        // NO timer
        acl.run(&graph, center, alpha, min(0.00001,epsilon), &scores_priv_corr);

        scores_priv_heur.clear();

        // Compute Heuristics
        stats_heur.begin_op();
        heur.run(center, alpha, scores_pub_neighbor, &scores_priv_heur);
        stats_heur.end_op();

        // Output Rankings
        output_rankings(scores_priv_corr, scores_priv_heur, output);

        // Rem Priv Edges
        for (const auto& leaf : leaves) {
            assert(graph.remove_edge(center, leaf));
        }
    }

    // Compute average times
    double corr_avg = 0.0;
    for (const auto& stat : stats_corr.stats()) {
        corr_avg += stat.micros;
    }
    corr_avg /= stats_corr.stats().size();

    double heur_avg = 0.0;
    for (const auto& stat : stats_heur.stats()) {
        heur_avg += stat.micros;
    }
    heur_avg /= stats_heur.stats().size();

    cout << "Avg Time Micros Heuristics: " << heur_avg << endl;
    cout << "Avg Time Micros Exact: " << corr_avg << endl;
    cout << "A/B: " << heur_avg / corr_avg << endl;

    return 0;
}
