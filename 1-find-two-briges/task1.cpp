//
// Created by radko on 7/2/17.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>

using namespace std;

using vertex = size_t;
using graph = vector<vector<vertex>>;
struct vertex_info {
    vertex parent;
    size_t tin;
    size_t fup;
    bool used;
};
using graph_info = vector<struct vertex_info>;
using edge = pair<vertex, vertex>;

graph parse_graph(string file_path) {
    ifstream ifs(file_path);

    size_t n;
    ifs >> n;

    graph res(n);

    for (size_t i = 0; i < n; ++i) {
        size_t k;
        ifs >> k;
        res[i].reserve(k);

        for (size_t j = 0; j < k; ++j) {
            size_t t;
            ifs >> t;

            res[i].emplace_back(t);
        }
    }

    ifs.close();
    return res;
}

void print_graph(const graph& g) {
    for (auto&& it : g) {
        for (auto && jt : it) {
            cout << jt << " ";
        }
        cout << boolalpha << endl;
    }
}

void dfs(const graph& g, vertex v, graph_info& info, vector<edge>& bridges) {
    static size_t timer = 0;
    if (!info[g[0][0]].used) {
        timer = 0;
    }

    for (auto neighbour : g[v]) {
        if (neighbour == info[v].parent) continue;

        if (!info[neighbour].used) {
            info[neighbour] = {v, ++timer, timer, true};
            dfs(g, neighbour, info, bridges);

            info[v].fup = min(info[v].fup, info[neighbour].fup);
            if (info[neighbour].fup > info[v].tin) {
                bridges.emplace_back(edge(v, neighbour));
            }
        } else {
            info[v].fup = min(info[v].fup, info[neighbour].tin);
        }
    }
}

pair<graph_info, vector<edge>> run_dfs(const graph& g) {
    graph_info g_info(g.size());
    vector<edge> bridges;
    g_info[0] = {0, 0, 0, true};
    dfs(g, 0, g_info, bridges);
    return pair<graph_info, vector<edge>>(g_info, bridges);
}

graph delete_edge(const graph& g, const vertex& u, const vertex& v) {
    graph g1 = g;

    for (auto it = g1[u].begin(); it != g1[u].end(); ++it) {
        if (*it == v) {
            g1[u].erase(it--);
        }
    }
    for (auto it = g1[v].begin(); it != g1[v].end(); ++it) {
        if (*it == u) {
            g1[v].erase(it--);
        }
    }

    return g1;
}

bool has_isolated(const graph& g) {
    return accumulate(g.begin(), g.end(), false, [](bool p, vector<vertex> x){ return p || x.empty(); });
}

vector<edge> solve_problem(const graph& g) {

    for (vertex i = 0; i < g.size(); ++i) {
        for (vertex j = i + 1; j < g.size(); ++j) {
            auto tmp_g = delete_edge(g, i, j);

            auto tmp_info = run_dfs(tmp_g);

            for (auto&& it : tmp_info.second) {
                if (!has_isolated(delete_edge(tmp_g, it.first, it.second))) {
                    return {edge(it.first, it.second), edge(i, j)};
                }
            }
        }
    }

    return vector<edge>();
};

void output_result(const string& filename, const vector<edge>& bridges) {
    ofstream ofs(filename);

    for (auto&& b : bridges) {
        ofs << b.first << " " << b.second << endl;
    }
    
    ofs.close();
}

int main(int argc, char** argv) {

    auto g = parse_graph("input.txt");

    auto bridges = solve_problem(g);

    output_result("output.txt", bridges);

    return 0;
}