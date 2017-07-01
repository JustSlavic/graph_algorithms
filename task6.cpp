//
// Created by radko on 6/30/17.
//

#include <fstream>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <tuple>

using namespace std;

typedef int vertex;
typedef vector<vector<vertex>> graph;

pair<unsigned, unsigned> read_data(string filename, vector<int>& stct_x, vector<int>& stct_y, vector<int>& power_x, vector<int>& power_y) {
    ifstream ifs(filename);

    unsigned n, k;

    ifs >> n >> k;

    int tmp;

    for (int i = 0; i < k; ++i) {
        ifs >> tmp;
        stct_x.emplace_back(tmp);
    }

    for (int j = 0; j < k; ++j) {
        ifs >> tmp;
        stct_y.emplace_back(tmp);
    }

    for (int i = 0; i < k; ++i) {
        ifs >> tmp;
        power_x.emplace_back(tmp);
    }

    for (int j = 0; j < k; ++j) {
        ifs >> tmp;
        power_y.emplace_back(tmp);
    }

    ifs.close();

    return make_pair(n, k);
}

/* numerating will be:
 *
 *     top layer  bot layer
 *      0 1 2     0  1  2
 *      ---c-->   ----c--->
 * 0 |  1 2 3   | 10 11 12
 * 1 r  4 5 6   r 13 14 15
 * 2 V  7 8 9   V 16 17 18
 *
 * top (r,c)  = r*n + c + 1
 * bot (r,c)  = r*n + c + 1 + n*n
 *
 * e.g. (2,1) = 2*3 + 1 + 1     = 8
 * and  (2,1) = 2*3 + 1 + 1 + 9 = 17
 */

graph build_graph(unsigned n, vector<int>& stct_r, vector<int>& stct_c, vector<int>& power_r, vector<int>& power_c) {
    const unsigned N = 2*n*n + 2;
    const int shift[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    graph g(N);

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            // nodes of top layer "see" only similar node of bottom layer
            g[r*n + c + 1].emplace_back(r*n + c + 1 + n*n);

            for (int i = 0; i < 4; ++i) {
                if (r + shift[i][0] < 0 || r + shift[i][0] >= n || c + shift[i][1] < 0 || c + shift[i][1] >= n)
                    continue;

                g[r*n + c + 1 + n*n].emplace_back((r + shift[i][0])*n + (c + shift[i][1]) + 1);
            }
        }
    }

    for (int i = 0; i < stct_r.size(); ++i) {
        // links fake source with stct points
        g[0].emplace_back(stct_r[i]*n + stct_c[i] + 1);
        // links power points with fake sink
        g[power_r[i]*n + power_c[i] + 1 + n*n].emplace_back(N - 1);
    }

    return g;
}

void print_graph(const graph& g) {
    for (int i = 0; i < g.size(); ++i) {
        cout << setw(2) << i << " | ";
        for (auto&& neighbour : g[i]) {
            cout << neighbour << " ";
        }
        cout << endl;
    }
}

void bfs(const graph& g, vertex v, vector<vertex>& parents) {
    queue<vertex> q;

    q.push(v);

    while (!q.empty()) {
        vertex u = q.front();
        q.pop();

        for (auto&& neighbour : g[u]) {
            if (parents[neighbour] == -1) {
                q.push(neighbour);
                parents[neighbour] = u;
            }
        }
    }
}

vector<vertex> run_bfs(const graph& g) {
    vector<vertex> parents(g.size(), -1);
    bfs(g, 0, parents);
    return parents;
}

graph build_residual(const graph& g, const graph& flow) {
    graph residual(g);

    for (vertex v = 0; v < flow.size(); ++v) {
        for (int i = 0; i < flow[v].size(); ++i) {
            for (auto it = residual[v].begin(); it != residual[v].end(); ++it) {
                if (*it == flow[v][i]) {
                    residual[v].erase(it);
                    residual[flow[v][i]].emplace_back(v);
                    break;
                }
            }
        }
    }

    return residual;
}

graph edmonds_karp(const graph& g) {
    /// 1. build residual graph
    /// 2. loop
    /// 3.   run bfs
    /// 4.   if there is no path -> return flow; else -> add flow;
    /// 5.   rebuild residual graph

    graph flow(g.size());
    graph residual(g);

    while (true) {
        vector<vertex> parents = run_bfs(residual);

        if (parents[g.size() - 1] == -1)
            return flow;

        // adding flow, assume u -> v
        vertex u = parents[g.size() - 1];
        vertex v = (vertex)g.size() - 1;
        do {
            for (int i = 0; i < g[u].size(); ++i) {
                if (g[u][i] == v) {
                    flow[u].emplace_back(v);
                    break;
                }
            }
            v = u;
            u = parents[u];
        }
        while (v != 0);

        residual = build_residual(g, flow);
    }
}

void output_result(string filename, graph& flow, const unsigned & n, const unsigned & k) {
    ofstream ofs(filename);

    if (flow[0].size() < k) {
        ofs << "-1";
        return;
    }

    while (!flow[0].empty()) {
        vertex u = flow[0][0];
        flow[0].erase(flow[0].begin());
        do {
            if (u > 0 && u < n*n)
                ofs << u - 1 << " ";

            u = flow[u][0];
        } while (u != flow.size() - 1);
        ofs << endl;
    }

    ofs.close();
}

int main() {

    vector<int> stct_r, stct_c, power_r, power_c; // stct = space and time continuum transgression

    unsigned n, k;
    tie(n, k) = read_data("input.txt", stct_r, stct_c, power_r, power_c);

    graph g = build_graph(n, stct_r, stct_c, power_r, power_c);

    print_graph(g); cout << endl;

    graph flow = edmonds_karp(g);

    print_graph(flow);

    output_result("output.txt", flow, n, k);

    return EXIT_SUCCESS;
}