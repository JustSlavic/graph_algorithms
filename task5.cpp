//
// Created by radko on 7/3/17.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <functional>
#include <iomanip>

using namespace std;

struct point {
    double x = 0;
    double y = 0;

    point () {}

    point (const double& _x, const double& _y) :x(_x), y(_y) {}
};

struct line {
    point p1, p2;

    line(const double& _x1, const double& _y1, const double& _x2, const double& _y2) {
        p1 = point(_x1, _y1);
        p2 = point(_x2, _y2);

        direct();
    }

    line(const point& _p1, const point& _p2) :p1(_p1), p2(_p2) {
        direct();
    }

    bool contains_in_rectangle(const point& p) const {
        return p.x >= p1.x && p.x <= p2.x && p.y >= p1.y && p.y <= p2.y;
    }

    void direct() {
        if (p1.x == p2.x) {
            if (p1.y > p2.y) {
                swap(p1, p2);
            }
        } else {
            if (p1.x > p2.x) {
                swap(p1, p2);
            }
        }
    }
};

struct edge {
    int to = -1;
    double c = 0;

    edge() {}

    edge(const int& _vertex, const double& _capacity) :to(_vertex), c(_capacity) {}
};

using graph = vector<vector<edge>>;

template <typename Key, typename Value>
struct pr_queue {
    using queue_pair = pair<Key, Value>;

    vector<queue_pair> container;
    std::function<bool(const queue_pair&, const queue_pair&)> cmp =
            [](const queue_pair& lhs, const queue_pair& rhs) { return lhs.first > rhs.first; };

    void push(const queue_pair& x) {
        container.push_back(x);
        push_heap(container.begin(), container.end(), cmp);
    }

    void push(const Key& k, const Value& val) {
        push(make_pair(k, val));
    }

    int top() {
        return container.front().second;
    }

    void pop() {
        pop_heap(container.begin(), container.end(), cmp);
        container.pop_back();
    }

    bool empty() {
        return container.empty();
    }

    void decrease_key(const queue_pair& x) {
        for (auto& t : container) {
            if (t.second == x.second) {
                t.first = x.first;
                make_heap(container.begin(), container.end(), cmp);
                return;
            }
        }

        push(x);
    }

    void decrease_key(const Key& k, const Value& val) {
        decrease_key(make_pair(k, val));
    }
};

ostream& operator<<(ostream& os, const point& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

ostream& operator<<(ostream& os, const line& l) {
    return os << l.p1 << " --- " << l.p2;
}

bool operator==(const point& lhs, const point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

void read_data(const string& filename, vector<line>& lines, point& home1, point& home2) {
    ifstream ifs(filename);

    unsigned N;
    ifs >> N;
    lines.reserve(N);

    for (int i = 0; i < N; ++i) {
        int tmp1, tmp2, tmp3, tmp4;
        ifs >> tmp1 >> tmp2 >> tmp3 >> tmp4;

        lines.emplace_back(line(tmp1, tmp2, tmp3, tmp4));
    }

    ifs >> home1.x >> home1.y >> home2.x >> home2.y;

    ifs.close();
}

graph build_graph(vector<line> lines) {
    const double eps = 10e-6;
    double cross_x, cross_y;

    vector<line> segments;

    for (int i = 0; i < lines.size(); ++i) {
        vector<point> segment_ends;

        for (int j = 0; j < lines.size(); ++j) {
            const double& x1 = lines[i].p1.x; const double& y1 = lines[i].p1.y;
            const double& x2 = lines[i].p2.x; const double& y2 = lines[i].p2.y;
            const double& x3 = lines[j].p1.x; const double& y3 = lines[j].p1.y;
            const double& x4 = lines[j].p2.x; const double& y4 = lines[j].p2.y;

            double divider = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

            if (fabs(divider) < eps) {
                cout << "lines " << lines[i] << " and " << lines[j] << " are parallel" << endl << endl;
            } else {
                cross_x = ((x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4)) / divider;
                cross_y = ((x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4)) / divider;

                if (lines[i].contains_in_rectangle(point(cross_x, cross_y)) &&
                        lines[j].contains_in_rectangle(point(cross_x, cross_y))) {
                    segment_ends.emplace_back(point(cross_x, cross_y));

                    cout << "cross = (" << cross_x << ", " << cross_y << ")" << endl;

                    const double vx1 = x2 - x1;
                    const double vy1 = y2 - y1;
                    const double vx2 = x4 - x3;
                    const double vy2 = y4 - y3;

                    const double cos_alpha = (vx1*vx2 + vy1*vy2) / (sqrt(vx1*vx1 + vy1*vy1) * sqrt(vx2*vx2 + vy2*vy2));
                    cout << "cos(alpha) = " << cos_alpha << endl;
                    cout << "measure of angle = " << 1 - cos_alpha << endl << endl;
                }
            }
        }

        sort(segment_ends.begin(), segment_ends.end(), [](const point& lhs, const point& rhs) {
            return (lhs.x == rhs.x ? lhs.y < rhs.y : lhs.x < rhs.x);
        });

        cout << "=== SEG ENDS of " << lines[i] << endl;
        for (auto&& p : segment_ends) {
            cout << "\t" << p << endl;
            segments.emplace_back(line(lines[i].p1, p));
            lines[i].p1 = p;
        }
        cout << endl;
    }

    for (auto&& l : lines) {
        segments.emplace_back(l);
    }

    cout << "=== SEGMENTS ===" << endl;
    for (auto&& s : segments) {
        cout << "\t" << s << endl;
    }
    cout << endl;

    graph g(segments.size());

    for (int i = 0; i < segments.size(); ++i) {
        for (int j = 0; j < segments.size(); ++j) {
            if (i != j && (segments[i].contains_in_rectangle(segments[j].p1) ||
                    segments[i].contains_in_rectangle(segments[j].p2))) {

                const double& x1 = segments[i].p1.x; const double& y1 = segments[i].p1.y;
                const double& x2 = segments[i].p2.x; const double& y2 = segments[i].p2.y;
                const double& x3 = segments[j].p1.x; const double& y3 = segments[j].p1.y;
                const double& x4 = segments[j].p2.x; const double& y4 = segments[j].p2.y;

                double vx1 = 0, vy1 = 0, vx2 = 0, vy2 = 0;

                /*
                 *  if     v1       v2
                 *      ------> * ------>
                 */
                if (segments[i].p2 == segments[j].p1) {
                    vx1 = x2-x1;
                    vy1 = y2-y1;
                    vx2 = x4-x3;
                    vy2 = y4-y3;
                }
                /*
                 *  if     v1        v2
                 *      ------> * <------
                 */
                if (segments[i].p2 == segments[j].p2) {
                    vx1 = x2-x1;
                    vy1 = y2-y1;
                    vx2 = x3-x4;
                    vy2 = y3-y4;
                }
                /*
                 *  if     v1       v2
                 *      <------ * ------>
                 */
                if (segments[i].p1 == segments[j].p1) {
                    vx1 = x1-x2;
                    vy1 = y1-y2;
                    vx2 = x4-x3;
                    vy2 = y4-y3;
                }
                /*
                 *  if     v1        v2
                 *      <------ * <------
                 */
                if (segments[i].p1 == segments[j].p2) {
                    vx1 = x1-x2;
                    vy1 = y1-y2;
                    vx2 = x3-x4;
                    vy2 = y3-y4;
                }

                const double cos_alpha = (vx1*vx2 + vy1*vy2) / (sqrt(vx1*vx1 + vy1*vy1) * sqrt(vx2*vx2 + vy2*vy2));
                cout << "\t\tangle(" << i << ", " << j << ") <- ";
                cout << "cos(alpha) = " << cos_alpha << "\t";
                cout << "measure of angle = " << 1 - cos_alpha << endl << endl;

                g[i].emplace_back(edge(j, 1 - cos_alpha));
            }
        }
    }

    cout << "=== GRAPH ===" << endl;
    for (int i = 0; i < g.size(); ++i) {
        cout << i << ": ";
        for (auto&& u : g[i]) {
            cout << "(" << u.to << ", " << setprecision(2) << u.c << ") ";
        }
        cout << endl;
    }

    return g;
}

void upgrade_graph(const graph& g) {
    graph upgraded_graph(2*g.size());


}

vector<int> dijkstra(const graph& g, const int& s) {
    vector<double> d(g.size(), INFINITY);
    d[s] = 0;
    vector<int> p(g.size(), -1);

    pr_queue<double, int> q;
    q.push(d[s], s);

    while (!q.empty()) {
        int v = q.top();
        q.pop();

        for (auto&& neighbour : g[v]) {
            if (d[neighbour.to] > d[v] + neighbour.c) {
                d[neighbour.to] = d[v] + neighbour.c;
                p[neighbour.to] = v;

                q.decrease_key(d[neighbour.to], neighbour.to);
            }
        }
    }

    return p;
}

int main() {

    vector<line> roads;
    point home1, home2;

    read_data("input.txt", roads, home1, home2);

    cout << "=== DATA ===" << endl;
    for (auto&& t : roads) {
         cout << t << endl;
    }
    cout << "home1 = " << home1 << endl;
    cout << "home2 = " << home2 << endl;

    cout << endl;
    graph g = build_graph(roads);

    vector<int> parents = dijkstra(g, 0);

    cout << "=== PARENTS ===" << endl;
    for (int i = 0; i < parents.size(); ++i) {
        cout << i << ": " << parents[i] << endl;
    }

    return 0;
}