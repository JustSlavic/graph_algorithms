//
// Created by radko on 7/2/17.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <tuple>

using namespace std;

struct vertex_info {
    size_t number;
    size_t leave_time;
    bool used;
};
using graph_info = vector<struct vertex_info>;

using vertex = size_t;
using graph = vector<vector<double>>;

graph parse_graph(const string& filename) {
    ifstream ifs(filename);

    size_t n;
    ifs >> n;

    graph g(n, vector<double>(n));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            ifs >> g[i][j];
        }
    }

    ifs.close();

    return g;
}

void topological_sort(const vertex& u, const graph& g, graph_info& info, list<vertex>& sorted) {
    static size_t timer = 0;
    info[u].used = true;
    for (size_t neighbour = 0; neighbour < g.size(); ++neighbour) {
        if (!info[neighbour].used && g[u][neighbour]) {
            topological_sort(neighbour, g, info, sorted);
        }
    }
    sorted.emplace_front(u);
    info[u].leave_time = timer++;
}

vector<vertex> run_topological_sort(const graph& g) {
    graph_info info(g.size());

    list<vertex> sorted_list;

    info[0] = {0,0,true};

    topological_sort(0, g, info, sorted_list);

    vector<vertex> sorted_vector;
    sorted_vector.reserve(sorted_list.size());

    for (auto&& v : sorted_list) {
        sorted_vector.emplace_back(v);
    }

    return sorted_vector;
}

pair<vector<vertex>, double> find_chance(graph g, const vector<vertex>& sorted) {
    vector<vertex> parents(g.size());

    for (size_t k = 1; k < sorted.size(); ++k) {
        for (size_t j = k+1; j < sorted.size(); ++j) {
            if (g[0][sorted[j]] < g[0][sorted[k]]*g[sorted[k]][sorted[j]]) {
                g[0][sorted[j]] = g[0][sorted[k]]*g[sorted[k]][sorted[j]];
                parents[sorted[j]] = sorted[k];
            }
        }
    }

    return make_pair(parents, g[0][g.size()-1]);
}

int main() {

    graph g = parse_graph("input.txt");

    vector<vertex> sorted = run_topological_sort(g);

    double chance;
    vector<vertex> parents;

    tie(parents, chance) = find_chance(g, sorted);

    vector<vertex> back_path;
    vertex stone = parents.size()-1;

    while (stone) {
        back_path.emplace_back(stone);
        stone = parents[stone];
    }
    back_path.emplace_back(0);

    ofstream ofs("output.txt");

    ofs << chance << " " << back_path.size() << endl;

    for (auto it = back_path.rbegin(); it != back_path.rend(); ++it) {
        ofs << *it << " ";
    }

    ofs.close();

    return 0;
}
