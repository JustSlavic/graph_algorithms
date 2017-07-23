//
// Created by radko on 7/3/17.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <tuple>
#include <map>
#include <chrono>

//#define time_checking
//#define vertex_print

using namespace std;

struct vertex {
    int x, y;
    template <class T>
    vertex(T _x, T _y) {
        x = static_cast<int>(_x);
        y = static_cast<int>(_y);
    }
};

struct edge {
    int from;
    int to;
    int capacity;
    edge(int u, int v, int c) :from(u), to(v), capacity(c) {}
};
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

bool operator<(const edge& lhs, const edge& rhs) {
    return lhs.capacity < rhs.capacity;
}

bool operator!=(const edge& lhs, const edge& rhs) {
    return lhs.from == rhs.from && lhs.to == rhs.to;
}

struct DSU {
    vector<int> parent;
    vector<int> rank;

    DSU(size_t _max_size) {
        parent.resize(_max_size, -1);
        rank.resize(_max_size, -1);
    }

    void make_set(const int& x) {
        if (parent[x] < 0) {
            parent[x] = x;
            rank[x] = 0;
        }
    }

    int find_set(int x) {
        while (parent[x] != x) {
            x = parent[x] = parent[parent[x]]; // эвристика сжатия пути
        }

        return x;
    }

    void union_sets(const int& lhs, const int& rhs) {
        int a = find_set(lhs);
        int b = find_set(rhs);

        if (a != b) {
            if (rank[a] > rank[b]) {
                parent[b] = a;
                rank[a]++;
            } else {
                parent[a] = b;
                rank[b]++;
            }
        }
    }

    void print() {
        for (int i = 0; i < parent.size(); ++i) {
            if (parent[i] >= 0)
                cout << "{" << i << " -> " << parent[i] << ", rank = " << rank[i] << ", tree = " << find_set(i) << "}" << endl;
        }
    }
};

unsigned edge_length(const vertex& u, const vertex& v) {
    return static_cast<unsigned>( abs(u.x - v.x + u.y - v.y) );
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
#ifdef time_checking
    auto start1 = chrono::steady_clock::now();
#endif //time_checking

    vector<vector<int>> h2(h);
    unsigned sum = 0;

    int vertex_num = 0;

    vector<int> h_nums(h.size());
    vector<int> v_nums(v.size());

#ifdef time_checking
    auto end1 = chrono::steady_clock::now();
    cout << "\tinit :     \t" << chrono::duration_cast<chrono::microseconds>(end1 - start1).count() << " us. " << endl;
    auto start2 = chrono::steady_clock::now();
#endif //time_checking

#ifdef vertex_print
    cout << "=== TOP VERTEXES === " << endl;
#endif //vertex_print

    for (int r = 0; r < h.size(); ++r) {
#ifdef vertex_print
        cout << "\th[" << r << "] <- " << vertex_num << endl;
#endif //vertex_print
        h_nums[r] = vertex_num++;
    }
    for (int c = 0; c < v.size(); ++c) {
#ifdef vertex_print
        cout << "\tv[" << c << "] <- " << vertex_num << endl;
#endif //vertex_print
        v_nums[c] = vertex_num++;
    }

#ifdef time_checking
    auto end2 = chrono::steady_clock::now();
    cout << "\tfst v nums : \t" << chrono::duration_cast<chrono::microseconds>(end2 - start2).count() << " us. " << endl;
    auto start3 = chrono::steady_clock::now();
    long init = 0, t1 = 0, t2 = 0, t3 = 0, last = 0;
#endif //time_checking

#ifdef vertex_print
    cout << "=== CROSSES ===" << endl;
#endif //vertex_print

    for (int r = 0; r < h.size(); ++r) {
        for (int c = 0; c < v.size(); ++c) {
#ifdef time_checking
            auto start_init = chrono::steady_clock::now();
#endif //time_checking

            vertex top_h(h[r][0], h[r][1]);
            vertex top_v(v[c][0], v[c][2]);
            vertex cross(top_h.x, top_v.y);
            int cross_num = 0;

#ifdef time_checking
            auto end_init = chrono::steady_clock::now();
            if (init < chrono::duration_cast<chrono::microseconds>(end_init - start_init).count()) {
                init = chrono::duration_cast<chrono::microseconds>(end_init - start_init).count();
            }
            auto start_1 = chrono::steady_clock::now();
#endif //time_checking

            if (cross.y >= top_h.y && cross.y <= h[r][2] && cross.x >= top_v.x && cross.x <= v[c][1]) {
#ifdef vertex_print
                cout << "\t(" << cross.x << ", " << cross.y << ")" << endl;
#endif //vertex_print

                sum++;
                cross_num = vertex_num++;
            }

#ifdef time_checking
            auto end_1 = chrono::steady_clock::now();
            if (t1 < chrono::duration_cast<chrono::microseconds>(end_1 - start_1).count()) {
                t1 = chrono::duration_cast<chrono::microseconds>(end_1 - start_1).count();
            }
            auto start_2 = chrono::steady_clock::now();
#endif //time_checking

            //check if cross point lays on the horizontal word
            if (cross.y >= top_h.y && cross.y <= h[r][2] && cross.x >= top_v.x && cross.x <= v[c][1]) {
                //if it is, add new edge to the graph and move left side of the word to the cross point
                g.emplace_back(edge(h_nums[r], cross_num, edge_length(top_h, cross)));
#ifdef vertex_print
                cout << "g <- (" << h_nums[r] << ", " << cross_num << ", " << edge_length(top_h, cross) << ")" << endl;
#endif //vertex_print
                h[r][1] = cross.y;
                h_nums[r] = cross_num;
            }

#ifdef time_checking
            auto end_2 = chrono::steady_clock::now();
            if (t2 < chrono::duration_cast<chrono::microseconds>(end_2 - start_2).count()) {
                t2 = chrono::duration_cast<chrono::microseconds>(end_2 - start_2).count();
            }
            auto start_3 = chrono::steady_clock::now();
#endif //time_checking

            //check if cross point lays on the vertical word
            if (cross.x >= top_v.x && cross.x <= v[c][1] && cross.y >= top_h.y && cross.y <= h2[r][2]) {
                //if it is, add new edge to the graph and move top side of the word to the cross point
                g.emplace_back(edge(v_nums[c], cross_num, edge_length(top_v, cross)));
#ifdef vertex_print
                cout << "g <- (" << v_nums[c] << ", " << cross_num << ", " << edge_length(top_v, cross) << ")" << endl;
#endif //vertex_print

                v[c][0] = cross.x;
                v_nums[c] = cross_num;
            }

#ifdef time_checking
            auto end_3 = chrono::steady_clock::now();
            if (t3 < chrono::duration_cast<chrono::microseconds>(end_3 - start_3).count()) {
                t3 = chrono::duration_cast<chrono::microseconds>(end_3 - start_3).count();
            }
#endif //time_checking
        }

#ifdef time_checking
        auto start_last = chrono::steady_clock::now();
#endif //time_checking

        //after all vertical checked push final edge of the horizontal word
        if (h[r][1] != h[r][2]) { // if "tail" if left, add edge to graph
#ifdef vertex_print
            cout << "g <- (" << h_nums[r] << ", " << vertex_num << ", " << edge_length(vertex(h[r][0], h[r][1]), vertex(h[r][0], h[r][2])) << ")" << endl;
#endif //vertex_print
            g.emplace_back(edge(h_nums[r], vertex_num++, edge_length(vertex(h[r][0], h[r][1]), vertex(h[r][0], h[r][2]))));
        }

#ifdef time_checking
        auto end_last = chrono::steady_clock::now();
        if (last < chrono::duration_cast<chrono::microseconds>(end_last - start_last).count()) {
            last = chrono::duration_cast<chrono::microseconds>(end_last - start_last).count();
        }
#endif //time_checking
    }

#ifdef time_checking
    auto end3 = chrono::steady_clock::now();
    cout << "\t\tinner init:\t" << init << endl;
    cout << "\t\tinner t1  :\t" << t1 << endl;
    cout << "\t\tinner t2  :\t" << t2 << endl;
    cout << "\t\tinner t3  :\t" << t3 << endl;
    cout << "\t\tinner last  :\t" << last << endl;

    cout << "\tmain loop : \t" << chrono::duration_cast<chrono::microseconds>(end3 - start3).count() << " us. " << endl;
    auto start4 = chrono::steady_clock::now();
#endif //time_checking

    for (int c = 0; c < v.size(); ++c) {
        //after all horizontal checked, push final edge of the vertical word
        if (v[c][0] != v[c][1]) { // if "tail" if left, add edge to graph
#ifdef vertex_print
            cout << "g <- (" << v_nums[c] << ", " << vertex_num << ", " << edge_length(vertex(v[c][0], v[c][2]), vertex(v[c][1], v[c][2])) << ")" << endl;
#endif //vertex_print
            g.emplace_back(edge(v_nums[c], vertex_num++, edge_length(vertex(v[c][0], v[c][2]), vertex(v[c][1], v[c][2]))));
        }
    }

#ifdef time_checking
    auto end4 = chrono::steady_clock::now();
    cout << "\tlast loop : \t" << chrono::duration_cast<chrono::microseconds>(end4 - start4).count() << " us. " << endl;
#endif //time_checking

    return sum;
}

unsigned kruskal(graph& g) {
    sort(g.begin(), g.end(), [](const edge& e1, const edge& e2) {
        return e1.capacity < e2.capacity;
    });

    DSU dsu(2*g.size());
    for (auto&& e : g) {
        dsu.make_set(e.from);
        dsu.make_set(e.to);
    }

    unsigned sum = 0;
    for (auto&& e : g) {
        if (dsu.find_set(e.from) != dsu.find_set(e.to)) {
            dsu.union_sets(e.from, e.to);
        } else {
            sum += e.capacity - 1;
        }
    }

    return sum;
}

void print_graph(graph& g) {
    std::cout << "=== GRAPH ===" << std::endl;
    for (auto&& e : g) {
        std::cout << e.from << " -> " << e.to << std::endl;
    }
}


void output_answer(string filename, int ans) {
    ofstream ofs(filename);

    ofs << ans << endl;

    ofs.close();
}

int main() {

#ifdef time_checking
    chrono::steady_clock::time_point start1 = chrono::steady_clock::now();
#endif //time_checking

    vector<vector<int>> h, v;

    int score = read_data("input.txt", h, v);

#ifdef time_checking
    chrono::steady_clock::time_point end1 = chrono::steady_clock::now();
    cout << "reading data:   " << chrono::duration_cast<chrono::microseconds>(end1 - start1).count() << " us. " << endl;
    chrono::steady_clock::time_point start2 = chrono::steady_clock::now();
#endif //time_checking

    sort(h.begin(), h.end(), [](const vector<int>& lhs, const vector<int>& rhs) {
        return (lhs[0] == rhs[0] ? lhs[1] < rhs[1] : lhs[0] < rhs[0]);
    });
    sort(v.begin(), v.end(), [](const vector<int>& lhs, const vector<int>& rhs) {
        return (lhs[2] == rhs[2] ? lhs[0] < rhs[0] : lhs[2] < rhs[2]);
    });

#ifdef time_checking
    chrono::steady_clock::time_point end2 = chrono::steady_clock::now();
    cout << "sorting:     \t" << chrono::duration_cast<chrono::microseconds>(end2 - start2).count() << " us. " << endl;
    chrono::steady_clock::time_point start3 = chrono::steady_clock::now();
#endif //time_checking

    graph g;
    g.reserve(10000000);

    int amount_crosses = parse(h, v, g);

#ifdef time_checking
    chrono::steady_clock::time_point end3 = chrono::steady_clock::now();
    cout << "parsing graph: \t" << chrono::duration_cast<chrono::microseconds>(end3 - start3).count() << " us. " << endl;
    chrono::steady_clock::time_point start4 = chrono::steady_clock::now();
#endif //time_checking

    int redundant_length = kruskal(g);

#ifdef time_checking
    chrono::steady_clock::time_point end4 = chrono::steady_clock::now();
    cout << "kruskal:     \t" << chrono::duration_cast<chrono::microseconds>(end4 - start4).count() << " us. " << endl;
    chrono::steady_clock::time_point start5 = chrono::steady_clock::now();
#endif //time_checking

    output_answer("output.txt", (redundant_length ? score - amount_crosses - redundant_length : -1));
//    output_answer("output.txt", -1);

#ifdef time_checking
    chrono::steady_clock::time_point end5 = chrono::steady_clock::now();
    cout << "printing data: \t" << chrono::duration_cast<chrono::microseconds>(end5 - start5).count() << " us. " << endl;
    cout << "\noverall      : \t" << chrono::duration_cast<chrono::microseconds>(end5 - start1).count() << " us. " << endl;
#endif //time_checking

    return EXIT_SUCCESS;
}