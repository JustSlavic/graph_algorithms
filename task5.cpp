//
// Created by radko on 7/3/17.
//

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

struct point {
    int x = 0;
    int y = 0;

    point () {}
    point (const int& _x, const int& _y) :x(_x), y(_y) {}
};

struct line {
    point p1, p2;

    line(const int& _x1, const int& _y1, const int& _x2, const int& _y2) {
        p1 = point(_x1, _y1);
        p2 = point(_x2, _y2);
    }
    line(const point& _p1, const point& _p2) :p1(_p1), p2(_p2) {}
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

int main() {

    vector<line> roads;
    point home1, home2;

    read_data("input.txt", roads, home1, home2);

    cout << "=== DATA ===" << endl;
    for (auto&& t : roads) {
         cout << t << endl;
    }

    return 0;
}