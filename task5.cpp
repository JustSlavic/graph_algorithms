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

const double eps = 10e-6;
const double pi = 3.14159265359;

struct point {
    double x = 0;
    double y = 0;

    point () {}

    point (const double& _x, const double& _y) :x(_x), y(_y) {}
};

double distance(const point& lhs, const point& rhs) {
    return sqrt((lhs.x - rhs.x)*(lhs.x - rhs.x) + (lhs.y - rhs.y)*(lhs.y - rhs.y));
}

struct line {
    point p1, p2;

    line(const double& _x1, const double& _y1, const double& _x2, const double& _y2) {
        p1 = point(_x1, _y1);
        p2 = point(_x2, _y2);
    }

    line(const point& _p1, const point& _p2) :p1(_p1), p2(_p2) {}

    bool number_between(double n, double l_bound, double r_bound) const {
        if (l_bound < r_bound) {
            return n >= l_bound && n <= r_bound;
        } else {
            return n <= l_bound && n >= r_bound;
        }
    }

    bool contains(const point& p) const {
        return ((p.x - p1.x)*(p2.y - p1.y) - (p.y - p1.y)*(p2.x - p1.x)) < eps;
    }

    bool contains_in_rectangle(const point& p) const {
        return number_between(p.x, p1.x, p2.x) && number_between(p.y, p1.y, p2.y);
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
    return os << setprecision(2) << "(" << p.x << ", " << p.y << ")";
}

ostream& operator<<(ostream& os, const line& l) {
    return os << l.p1 << " --- " << l.p2;
}

bool operator==(const point& lhs, const point& rhs) {
    return fabs(lhs.x - rhs.x) < eps && fabs(lhs.y - rhs.y) < eps;
}

bool operator!=(const point& lhs, const point& rhs) {
    return !(lhs == rhs);
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

/*
 * the idea in build graph is
 *  1. make segments of the lines vertices
 *  2. separate each vertex to 2 individual vertices
 *  3. link them in a way all paths persist, and you can't turn for 360 degrees
 *
 *  the way I link vertices is given below
 */

// lines in will be destroyed during building, so I copy it
graph build_graph(vector<line> lines, const point& home1, const point& home2) {
    vector<line> segments;
    vector<vector<point>> intersections(lines.size());

    for (int i = 0; i < lines.size(); ++i) {
        for (int j = 0; j < lines.size(); ++j) {
            const double& x1 = lines[i].p1.x; const double& y1 = lines[i].p1.y;
            const double& x2 = lines[i].p2.x; const double& y2 = lines[i].p2.y;
            const double& x3 = lines[j].p1.x; const double& y3 = lines[j].p1.y;
            const double& x4 = lines[j].p2.x; const double& y4 = lines[j].p2.y;

            double divider = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

            if (fabs(divider) >= eps) {
                double cross_x = ((x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4)) / divider;
                double cross_y = ((x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4)) / divider;

                point cross = point(cross_x, cross_y);

                if (lines[i].contains_in_rectangle(cross) && lines[j].contains_in_rectangle(cross)) {
                    intersections[i].emplace_back(cross);
                }
            }
        }
    }

    cout << "=== ALL CROSSES ===" << endl;
    for (int i = 0; i < intersections.size(); ++i) {
        cout << i << ":\t";
        for (auto&& p : intersections[i]) {
            cout << p << " ";
        }
        cout << endl;
    }

    for (int i = 0; i < lines.size(); ++i) {
        std::function<bool(const point&, const point&)> cmp =
                [&lines, &i](const point& lhs, const point& rhs) -> bool {
                    return distance(lines[i].p1, lhs) < distance(lines[i].p1, rhs);
                };

        sort(intersections[i].begin(), intersections[i].end(), cmp);

        for (auto&& intersect_point : intersections[i]) {
            if (lines[i].p1 != intersect_point) {
                segments.emplace_back(line(lines[i].p1, intersect_point));
                lines[i].p1 = intersect_point;
            }
        }

        if (lines[i].p1 != lines[i].p2)
            segments.emplace_back(lines[i]);
    }

    cout << "=== SEGMENTS ===" << endl;
    for (auto&& s : segments) {
        cout << "\t" << s << endl;
    }
    cout << endl;

    const unsigned n = static_cast<unsigned>(segments.size());
    graph g(n*2 + 2);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
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
                 *
                 *  then
                 *
                 *      v1             v2
                 *    | fst |  --->  | fst |
                 *    | snd |  <---  | snd |
                 *
                 */
                if (segments[i].p2 == segments[j].p1) {
                    vx1 = x2-x1;
                    vy1 = y2-y1;
                    vx2 = x4-x3;
                    vy2 = y4-y3;

                    const double angle = acos((vx1*vx2 + vy1*vy2) / (sqrt(vx1*vx1 + vy1*vy1) * sqrt(vx2*vx2 + vy2*vy2)))*180 / pi;

                    g[i].emplace_back(edge(j, angle));
                    g[n + j].emplace_back(edge(n + i, angle));
                }
                /*
                 *  if     v1        v2
                 *      ------> * <------
                 *
                 *  then
                 *
                 *      v1             v2
                 *    | fst |  --->  | snd |
                 *    | snd |  <---  | fst |
                 *
                 */
                if (segments[i].p2 == segments[j].p2) {
                    vx1 = x2-x1;
                    vy1 = y2-y1;
                    vx2 = x3-x4;
                    vy2 = y3-y4;

                    const double angle = acos((vx1*vx2 + vy1*vy2) / (sqrt(vx1*vx1 + vy1*vy1) * sqrt(vx2*vx2 + vy2*vy2)))*180 / pi;

                    g[i].emplace_back(edge(n + j, angle));
                    g[j].emplace_back(edge(n + i, angle));
                }
                /*
                 *  if     v1       v2
                 *      <------ * ------>
                 *
                 *  then
                 *
                 *       v1             v2
                 *     | snd |  --->  | fst |
                 *     | fst |  <---  | snd |
                 *
                 */
                if (segments[i].p1 == segments[j].p1) {
                    vx1 = x1-x2;
                    vy1 = y1-y2;
                    vx2 = x4-x3;
                    vy2 = y4-y3;

                    const double angle = acos((vx1*vx2 + vy1*vy2) / (sqrt(vx1*vx1 + vy1*vy1) * sqrt(vx2*vx2 + vy2*vy2)))*180 / pi;

                    g[n + i].emplace_back(edge(j, angle));
                    g[n + j].emplace_back(edge(i, angle));
                }
                /*
                 *  if     v1        v2
                 *      <------ * <------
                 *
                 *  then
                 *
                 *        v1             v2
                 *      | snd |  --->  | snd |
                 *      | fst |  <---  | fst |
                 *
                 */
                if (segments[i].p1 == segments[j].p2) {
                    vx1 = x1-x2;
                    vy1 = y1-y2;
                    vx2 = x3-x4;
                    vy2 = y3-y4;

                    const double angle = acos((vx1*vx2 + vy1*vy2) / (sqrt(vx1*vx1 + vy1*vy1) * sqrt(vx2*vx2 + vy2*vy2)))*180 / pi;

                    g[n + i].emplace_back(edge(n + j, angle));
                    g[j].emplace_back(edge(i, angle));
                }
            }
        }

        if (segments[i].contains(home1) && segments[i].contains_in_rectangle(home1)) {
            g[2*n].emplace_back(edge(i, 0));
            g[i].emplace_back(edge(2*n, 0));
            g[2*n].emplace_back(edge(n + i, 0));
            g[n + i].emplace_back(2*n, 0);
        }

        if (segments[i].contains(home2) && segments[i].contains_in_rectangle(home2)) {
            g[2*n + 1].emplace_back(edge(i, 0));
            g[i].emplace_back(2*n + 1, 0);
            g[2*n + 1].emplace_back(edge(n + i, 0));
            g[n + i].emplace_back(2*n + 1, 0);
        }
    }

    cout << "=== GRAPH ===" << endl;
    for (int i = 0; i < g.size(); ++i) {
        cout << i << ": ";
        for (auto&& u : g[i]) {
            cout << "(" << u.to << ", " << setprecision(3) << (u.c < eps ? 0 : u.c) << ") ";
        }
        cout << endl;
    }

    return g;
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

double count_angle(const graph& g, const vector<int>& parents) {

    int u = static_cast<int>(parents.size() - 1);
    int parent = parents[u];
    double angle = 0;

    while (parent != parents.size() - 2) {
        for (auto&& e : g[parent]) {
            if (e.to==u) {
                angle += e.c;
                break;
            }
        }
        u = parent;
        parent = parents[u];
    }

    return angle;
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
    graph g = build_graph(roads, home1, home2);

    vector<int> parents = dijkstra(g, static_cast<int>(g.size()-2));

    cout << "=== PARENTS ===" << endl;
    for (int i = 0; i < parents.size(); ++i) {
        cout << i << ": " << parents[i] << endl;
    }

    cout << "=== ANGLE ===" << endl;
    cout << "angle = " << count_angle(g, parents) << endl;

    return 0;
}