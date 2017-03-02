

#include "GraphUtils.h"
#include <cstring>
#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;

void import_graph(const string& file_name, UDynGraphMemEff* graph) {
    graph->clear();
    ifstream file_stream(file_name.c_str(), ios_base::in);
    std::string delimiter = " ";
    string line;

    int count = 0;

    vector<int> tokens;

    assert(file_stream.is_open());

    while (getline(file_stream, line)) {
        if (count % 10000000 == 0) {
            cout << "Imported: " << count / 1000000 << "M" << endl;
            cout.flush();
        }
        count++;

        tokens.clear();

        size_t pos = 0;
        std::string token;
        while (pos != line.npos) {
            pos = line.find(delimiter);
            token = line.substr(0, pos);
            int token_int = atoi(token.c_str());
            assert(token_int >= 0);
            tokens.push_back(token_int);
            line.erase(0, pos + delimiter.length());
        }
        if (line.size() == 0) { // last line
            break;
        }

        graph->add_edge(tokens[0], tokens[1]);

    }

    assert(graph->num_edges() > 0);
}
void import_private_graphs(const string& file_name,
    vector<pair<int, vector<int>>>* vec) {
        vec->clear();
        ifstream file_stream(file_name.c_str(), ios_base::in);
        std::string delimiter = " ";
        string line;

        vector<int> tokens;
        assert(file_stream.is_open());

        bool odd_line = true;
        string line_cp;

        while (getline(file_stream, line)) {
            line_cp = line;
            tokens.clear();

            size_t pos = 0;
            std::string token;
            while (pos != line.npos) {
                pos = line.find(delimiter);
                token = line.substr(0, pos);
                int token_int = atoi(token.c_str());
                assert(token_int >= 0);
                tokens.push_back(token_int);
                line.erase(0, pos + delimiter.length());
            }

            if (line.size() == 0) {
                continue;
            }

            if (odd_line) {
                assert(tokens.size() == 1);
                vec->emplace_back(); // add new item
                vec->back().first = tokens[0];
            } else {
                assert(tokens.size() >= 1);
                vec->back().second.insert(
                vec->back().second.end(), tokens.begin(),
                tokens.end());
            }

            odd_line = !odd_line;
        }

        assert(vec->size() > 0);
    }
