//
// Created by radko on 7/3/17.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

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
    }
    line(const point& _p1, const point& _p2) :p1(_p1), p2(_p2) {}
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

void build_graph(const vector<line>& lines) {
    const double eps = 10e-6;
    double cross_x, cross_y;

    for (int i = 0; i < lines.size(); ++i) {
        for (int j = i+1; j < lines.size(); ++j) {
            const double& x1 = lines[i].p1.x; const double& y1 = lines[i].p1.y;
            const double& x2 = lines[i].p2.x; const double& y2 = lines[i].p2.y;
            const double& x3 = lines[j].p1.x; const double& y3 = lines[j].p1.y;
            const double& x4 = lines[j].p2.x; const double& y4 = lines[j].p2.y;

            double divider = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

            if (fabs(divider) < eps) {
                cout << "lines " << lines[i] << " and " << lines[j] << " is parallel" << endl;
            } else {
                cross_x = ((x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4)) / divider;
                cross_y = ((x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4)) / divider;

                if (lines[i].contains_in_rectangle(point(cross_x, cross_y)) &&
                        lines[j].contains_in_rectangle(point(cross_x, cross_y))) {
                    cout << "cross = (" << cross_x << ", " << cross_y << ")" << endl;
                }
            }
        }
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