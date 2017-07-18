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
class union_sets {
public:
    void insert(const T& t) {
        for (auto&& s : sets) {
            if (s.find(t) != s.end()) {
                return;
            }
        }
        insert_singleton(t);
    }

    void unite(const T& t1, const T& t2) {
        if (in_one_set(t1, t2)) {
            return;
        }
        auto found_it = sets.end();
        auto found_jt = sets.end();

        for (auto it = sets.begin(), jt = sets.begin();
             it != sets.end() && jt != sets.end();
             ++it, ++jt) {
            if ((*it).find(t1) != (*it).end()) {
                found_it = it;
            }
            if ((*jt).find(t2) != (*jt).end()) {
                found_jt = jt;
            }
            if (found_it != sets.end() && found_jt != sets.end()) {
                break;
            }
        }
        if (found_it == sets.end() && found_jt == sets.end()) {
            sets.push_back({t1, t2});
            return;
        }
        if (found_it == sets.end()) {
            (*found_jt).insert(t2);
        }
        if (found_jt == sets.end()) {
            (*found_it).insert(t1);
        }

        for (auto&& t : *found_jt) {
            (*found_it).insert(t);
        }
        sets.erase(found_jt);
    }

    bool in_one_set(const T& t1, const T& t2) {
        for (auto&& s : sets) {
            auto s_in1 = s.find(t1);
            auto s_in2 = s.find(t2);
            if (s_in1 != s.end() && s_in2 != s.end()) {
                return true;
            }
        }
        return false;
    }

    void print(std::ostream& os) {
        for (auto&& s : sets) {
            os << "set: { ";
            for (auto&& t : s) {
                os << t << ", ";
            }
            os << " }" << std::endl;
        }
    }
private:
    void insert_singleton(const T& t) {
        sets.push_back({t});
    }

private:
    vector<set<T>> sets;
};

unsigned edge_length(edge e) {
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

unsigned kraskal(graph& g) {
    sort(g.begin(), g.end(), [](edge e1, edge e2) {
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

    union_sets<vertex> us;
    for (auto&& e : g) {
        us.insert(e.first);
        us.insert(e.second);
    }

    set<edge> tree;
    for (auto&& e : g) {
        if (us.in_one_set(e.first, e.second)) {
            continue;
        }
        us.unite(e.first, e.second);
        tree.insert(e);
    }

//#define treeprint

#ifdef treeprint
    cout << " === UNION SETS === " << endl;
    us.print(cout); cout << endl;

    cout << " === TREE === " << endl;
    for (auto&& e : tree) {
        cout << "\t" << e.first << " -- " << e.second << endl;
    }
    cout << endl;
#endif //tree

//#define missing


    unsigned sum = 0;
#ifdef missing
    cout << " === MISSING EDGES === " << endl;
#endif //missing
    for (auto&& e : g) {
        if (tree.find(e) == tree.end()) {
#ifdef missing
            cout << e.first << " --- " << e.second << endl;
#endif //missing
            sum += edge_length(e) - 1;
        }
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

    unsigned redundant_length = kraskal(g);

    output_answer("output.txt", score - amount_crosses - redundant_length);

    return 0;
}