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

pair<unsigned, unsigned> read_data(string filename, vector<vector<int>>& pins) {
    
    ifstream ifs(filename);

    unsigned n, k;

    ifs >> n >> k;

    pins.resize(4);

    int tmp;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j<k; ++j) {
            ifs >> tmp;
            pins[i].emplace_back(tmp);
        }
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

graph build_graph(unsigned n, vector<vector<int>>& pins) {
    const unsigned N = 2*n*n + 2;
    const int shift[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    graph g(N);

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            // nodes of top layer "see" only similar node of bottom layer
            g[r*n + c + 1].emplace_back(r*n + c + 1 + n*n);

            for (int i = 0; i < 4; ++i) {
                bool out_of_range = r + shift[i][0] < 0 || r + shift[i][0] >= n || c + shift[i][1] < 0 || c + shift[i][1] >= n;

                if (!out_of_range)
                    g[r*n + c + 1 + n*n].emplace_back((r + shift[i][0])*n + (c + shift[i][1]) + 1);
            }
        }
    }

    for (int i = 0; i < pins[0].size(); ++i) {
        // links fake source with stct points
        g[0].emplace_back(pins[0][i]*n + pins[1][i] + 1);
        // links power points with fake sink
        g[pins[2][i]*n + pins[3][i] + 1 + n*n].emplace_back(N - 1);
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
                    if (u) flow[u].clear(); // from fake source can run multiple flows, but not from real ones
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

void output_result(
        string filename,
        graph& flow,
        const unsigned & n,
        const unsigned & k) {

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

    vector<vector<int>> pins;
    unsigned n, k;

    tie(n, k) = read_data("input.txt", pins);

    graph g = build_graph(n, pins);

    graph flow = edmonds_karp(g);

    output_result("output.txt", flow, n, k);

    return EXIT_SUCCESS;
}