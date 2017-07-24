//
// Created by radko on 7/3/17.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>

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
        bool res = true;
        if (p1.x > p2.x) {
            res = res && p.x >= p2.x && p.x <= p1.x;
        } else {
            res = res && p.x >= p1.x && p.x <= p2.x;
        }
        if (p1.y > p2.y) {
            res = res && p.y >= p2.y && p.y <= p1.y;
        } else {
            res = res && p.y >= p1.y && p.y <= p2.y;
        }

        return res;
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

ostream& operator<<(ostream& os, const point& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

ostream& operator<<(ostream& os, const line& l) {
    return os << l.p1 << " --- " << l.p2;
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

void build_graph(vector<line> lines) {
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

    vector<vector<int>> g(segments.size());

    for (int i = 0; i < segments.size(); ++i) {
        for (int j = 0; j < segments.size(); ++j) {
            if (i != j && (segments[i].contains_in_rectangle(segments[j].p1) ||
                    segments[i].contains_in_rectangle(segments[j].p2))) {
                g[i].emplace_back(j);
            }
        }
    }

    cout << "=== GRAPH ===" << endl;
    for (int i = 0; i < g.size(); ++i) {
        cout << i << ": ";
        for (auto&& u : g[i]) {
            cout << u << " ";
        }
        cout << endl;
    }
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
    build_graph(roads);

    return 0;
}