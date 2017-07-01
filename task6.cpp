//
// Created by radko on 6/30/17.
//

#include <fstream>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

typedef int vertex;
typedef vector<vector<vertex>> graph;

unsigned read_data(string filename, vector<int>& stct_x, vector<int>& stct_y, vector<int>& power_x, vector<int>& power_y) {
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

    return n;
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

graph build_graph(unsigned n, vector<int>& stct_x, vector<int>& stct_y, vector<int>& power_x, vector<int>& power_y) {
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

    for (int i = 0; i < stct_x.size(); ++i) {
        g[0].emplace_back(stct_x[i]*n + stct_y[i] + 1);
        g[power_x[i]*n + power_y[i] + 1 + n*n].emplace_back(N - 1);
    }

    return g;
}

void print_graph(const graph& g) {
    for (auto&& node : g) {
        for (auto&& neighbour : node) {
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
            if (parents[neighbour] != -1) {
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

graph edmonds_karp(const graph& g) {
    /// loop
    /// 1. build residual graph
    /// 2. run bfs
    /// 3. if there is no path -> return flow; else -> build additional path;
    /// 4. add flow

    graph flow(g.size());
    graph residual(g);

    vector<vertex> parents = run_bfs(g);

    if (parents[g.size() - 1] == -1)
        return flow;
    
}

int main() {

    vector<int> stct_x, stct_y, power_x, power_y; // stct = space and time continuum transgression

    unsigned n = read_data("input.txt", stct_x, stct_y, power_x, power_y);

    graph g = build_graph(n, stct_x, stct_y, power_x, power_y);

    print_graph(g);

    return EXIT_SUCCESS;
}