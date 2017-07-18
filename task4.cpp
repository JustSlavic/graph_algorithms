//
// Created by radko on 7/3/17.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <tuple>

using namespace std;

struct vertex {
    int x, y;
    template <class T>
    vertex(T _x, T _y) {
        x = static_cast<int>(_x);
        y = static_cast<int>(_y);
    }
};
typedef pair<vertex, vertex> edge;
typedef vector<edge> graph;

ostream& operator<<(ostream& os, const vertex& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}
bool operator<(const vertex& lhs, const vertex& rhs) {
    return lhs.x == rhs.x ? lhs.y < rhs.y : lhs.x < rhs.x;
}
bool operator==(const vertex& lhs, const vertex& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
bool operator!=(const vertex& lhs, const vertex& rhs) {
    return ! (lhs == rhs);
}

template <class T>
struct DSU {
    vector<T> forest;
    vector<int> parent;

    void make_set(const T& x) {
        for (auto&& t : forest) {
            if (t == x) return;
        }

        forest.emplace_back(x);
        parent.emplace_back(parent.size());
    }

    T find_set(const T& x) {
        int i = 0;
        for (; i < forest.size(); ++i) { // поиск индекса элемента
            if (forest[i] == x) {
                break;
            }
        }

        while (i != parent[i]) { // поиск лидера множества
            i = parent[i] = parent[parent[i]];
        }

        return forest[i]; // возвращение лидера
    }

    void union_sets(const T& lhs, const T& rhs) {
        int i = -1;
        int j = -1;
        for (int k = 0; k < forest.size(); ++k) {
            if (forest[k] == lhs) i = k;
            if (forest[k] == rhs) j = k;
            if (i != -1 && j != -1) break;
        }

        while (i != parent[i] || j != parent[j]) {
            i = parent[i];
            j = parent[j];
        }

        parent[j] = i;
    }

    void print() {
        for (int i = 0; i < forest.size(); ++i) {
            cout << "{ " << forest[i] << " :-> " << find_set(forest[i]) << " };" << endl;
        }
    }
};

unsigned edge_length(const edge& e) {
    return static_cast<unsigned>( abs(e.first.x - e.second.x + e.first.y - e.second.y) );
}

unsigned read_data(const string& filename, vector<vector<int>>& h, vector<vector<int>>& v) {
    ifstream ifs(filename);

    unsigned h_amount, v_amount, sum = 0;
    ifs >> h_amount >> v_amount;

    for (int r = 0; r < h_amount; ++r) {
        int tmp1, tmp2, tmp3;
        ifs >> tmp1 >> tmp2 >> tmp3;
        h.emplace_back(vector<int>({tmp1, tmp2, tmp3}));
        sum += abs(tmp3 - tmp2) + 1;
    }

    for (int c = 0; c < v_amount; ++c) {
        int tmp1, tmp2, tmp3;
        ifs >> tmp1 >> tmp2 >> tmp3;
        v.emplace_back(vector<int>({tmp1, tmp2, tmp3}));
        sum += abs(tmp2 - tmp1) + 1;
    }

    ifs.close();

    return sum;
}

unsigned parse(vector<vector<int>>& h, vector<vector<int>>& v, graph& g) {
    vector<vector<int>> h2(h);
    unsigned sum = 0;

    for (int r = 0; r < h.size(); ++r) {
        for (int c = 0; c < v.size(); ++c) {
            vertex cross(h[r][0], v[c][2]);

            if (cross.y >= h[r][1] && cross.y <= h[r][2] && cross.x >= v[c][0] && cross.x <= v[c][1]) {
//                cout << "horizontal cross: " << cross << endl;
                sum++;
            }

            //check if cross point lays on the horizontal word
            if (cross.y > h[r][1] && cross.y < h[r][2] && cross.x >= v[c][0] && cross.x <= v[c][1]) {
                //if it is, add new edge to the graph and move left side of the word to the cross point
                g.emplace_back(make_pair(vertex(h[r][0], h[r][1]), cross));
                h[r][1] = cross.y;
            }

            //check if cross point lays on the vertical word
            if (cross.x > v[c][0] && cross.x < v[c][1] && cross.y >= h2[r][1] && cross.y <= h2[r][2]) {
//                cout << "vertical cross: " << cross << endl;
                //if it is, add new edge to the graph and move top side of the word to the cross point
                g.emplace_back(make_pair(vertex(v[c][0], v[c][2]), cross));
                v[c][0] = cross.x;
            }
        }
        //after all vertical checked push final edge of the horizontal word
        g.emplace_back(make_pair(vertex(h[r][0], h[r][1]), vertex(h[r][0], h[r][2])));
    }

    for (int c = 0; c < v.size(); ++c) {
        //after all horizontal checked, push final edge of the vertical word
        g.emplace_back(make_pair(vertex(v[c][0], v[c][2]), vertex(v[c][1], v[c][2])));
    }

    cout << endl;

    return sum;
}

unsigned kruskal(graph& g) {
    sort(g.begin(), g.end(), [](const edge& e1, const edge& e2) {
        return edge_length(e1) < edge_length(e2);
    });

//#define sorted

#ifdef sorted
    cout << " === SORTED EDGES === " << endl;
    for (auto&& e : g) {
        cout << "(" << e.first.x << ", " << e.first.y << ") -- (" << e.second.x << ", " << e.second.y << ")" << endl;
    }
    cout << endl;
#endif //sorted

    DSU<vertex> dsu;
    for (auto&& e : g) {
        dsu.make_set(e.first);
        dsu.make_set(e.second);
    }

    set<edge> tree;
    for (auto&& e : g) {
        if (dsu.find_set(e.first) != dsu.find_set(e.second)) {
            dsu.union_sets(e.first, e.second);
            tree.insert(e);
        }
    }

//#define treeprint
#ifdef treeprint
    cout << " === UNION SETS === " << endl;
    dsu.print(); cout << endl;

    cout << " === TREE === " << endl;
    for (auto&& e : tree) {
        cout << "\t" << e.first << " -- " << e.second << endl;
    }
    cout << endl;
#endif //tree

    unsigned sum = 0;
    for (auto&& e : g) {
        if (tree.find(e) == tree.end())
            sum += edge_length(e) - 1;
    }

    return sum;
}

void print_graph(graph& g) {
    std::cout << "=== GRAPH ===" << std::endl;
    for (auto&& e : g) {
        std::cout << e.first << " -> " << e.second << std::endl;
    }
}


void output_answer(string filename, int ans) {
    ofstream ofs(filename);

    ofs << ans << endl;

    ofs.close();
}

int main() {

    vector<vector<int>> h, v;

    unsigned score = read_data("input.txt", h, v);

    sort(h.begin(), h.end(), [](const vector<int>& lhs, const vector<int>& rhs) {
        return (lhs[0] == rhs[0] ? lhs[1] < rhs[1] : lhs[0] < rhs[0]);
    });
    sort(v.begin(), v.end(), [](const vector<int>& lhs, const vector<int>& rhs) {
        return (lhs[2] == rhs[2] ? lhs[0] < rhs[0] : lhs[2] < rhs[2]);
    });

    graph g;

    unsigned amount_crosses = parse(h, v, g);

    unsigned redundant_length = kruskal(g);

    output_answer("output.txt", score - amount_crosses - redundant_length);

    return 0;
}