//
// Created by radko on 7/2/17.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <tuple>

using namespace std;

typedef vector<vector<unsigned int>> labyrinth;
typedef tuple<unsigned int, unsigned int, unsigned int, unsigned int> vertex;

tuple<labyrinth, labyrinth, vertex> parse_labyrinth(string filename) {
    ifstream ifs(filename);

    unsigned int n, r1, c1, r2, c2;

    ifs >> n >> r1 >> c1 >> r2 >> c2;

    labyrinth lab1(n);
    labyrinth lab2(n);

    for (int r = 0; r < n; ++r) {
        lab1[r].resize(n);
        for (int c = 0; c < n; ++c) {
            ifs >> lab1[r][c];
        }
    }

    for (int r = 0; r < n; ++r) {
        lab2[r].resize(n);
        for (int c = 0; c < n; ++c) {
            ifs >> lab2[r][c];
        }
    }

    ifs.close();

    return make_tuple(lab1, lab2, make_tuple(r1, c1, r2, c2));
}

int bfs(const labyrinth& lab1, const labyrinth& lab2, vertex v) {
    queue<vertex> q;
    q.push(v);

    const int shifts[4][2] = { {0,1}, {1,0}, {0,-1}, {-1,0} };

    unsigned int _r1, _c1, _r2, _c2;
    tie(_r1, _c1, _r2, _c2) = v;

    size_t n = lab1.size();

    unsigned int levels[n][n][n][n];

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                for (int l = 0; l < n; ++l) {
                    levels[i][j][k][l] = 0;
                }
            }
        }
    }

    levels[_r1][_c1][_r2][_c2] = 1;

    while (!q.empty()) {
        auto& u = q.front();
        tie(_r1, _c1, _r2, _c2) = u;

        for (unsigned int shift = 0; shift < 4; ++shift) { // for adjacent vertexes
            unsigned int __r1 = _r1 + shifts[shift][0];
            unsigned int __c1 = _c1 + shifts[shift][1];
            unsigned int __r2 = _r2 + shifts[shift][0];
            unsigned int __c2 = _c2 + shifts[shift][1];

            if (__r1 >= n) __r1 = _r1;
            if (__c1 >= n) __c1 = _c1;
            if (__r2 >= n) __r2 = _r2;
            if (__c2 >= n) __c2 = _c2;

            if (lab1[__r1][__c1] == 1) {
                __r1 = _r1;
                __c1 = _c1;
            }
            if (lab2[__r2][__c2] == 1) {
                __r2 = _r2;
                __c2 = _c2;
            }
            if (__r1 == _r1 && __c1 == _c1 && __r2 == _r2 && __c2 == _c2) {
                continue;
            }

            if (levels[__r1][__c1][__r2][__c2] == 0) {
                q.push(make_tuple(__r1, __c1, __r2, __c2));
                levels[__r1][__c1][__r2][__c2] = levels[_r1][_c1][_r2][_c2] + 1;
            }
        }
        q.pop();
    }

    return levels[0][0][0][0] - 1;
};

int main() {

    labyrinth lab1, lab2;
    vertex v;
    tie(lab1, lab2, v) = parse_labyrinth("input.txt");

    int result = bfs(lab1, lab2, v);

    ofstream ofs("output.txt");
    ofs << result << endl;
    ofs.close();

    return 0;
}
