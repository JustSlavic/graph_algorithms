#include <iostream>
#include <cstring>
#include <assert.h>
#include <vector>
#include <fstream>
#include <queue>
#include <tuple>


using maze = std::vector<std::vector<unsigned>>;
using vertex = std::tuple<unsigned, unsigned, unsigned, unsigned>;


std::tuple<maze, maze, vertex> parse_maze(const std::string& filename) {
    std::ifstream ifs(filename);

    unsigned n = 0;
    unsigned r1 = 0;
    unsigned c1 = 0;
    unsigned r2 = 0;
    unsigned c2 = 0;

    ifs >> n >> r1 >> c1 >> r2 >> c2;

    assert(2 <= n && n <= 100);
    assert(r1 < n);
    assert(c1 < n);
    assert(r2 < n);
    assert(c2 < n);

    maze maze_1(n);
    maze maze_2(n);

    for (unsigned r = 0; r < n; ++r) {
        maze_1[r].resize(n);
        for (unsigned c = 0; c < n; ++c) {
            ifs >> maze_1[r][c];
        }
    }

    for (unsigned r = 0; r < n; ++r) {
        maze_2[r].resize(n);
        for (unsigned c = 0; c < n; ++c) {
            ifs >> maze_2[r][c];
        }
    }

    return std::make_tuple(maze_1, maze_2, std::make_tuple(r1, c1, r2, c2));
}


int bfs(const maze& maze_1, const maze& maze_2, vertex v) {
    if (maze_1.size() == 0 || maze_2.size() == 0) return -1;

    std::queue<vertex> q;
    q.push(v);

    const int shifts[4][2] = { {0,1}, {1,0}, {0,-1}, {-1,0} };

    unsigned r1, c1, r2, c2;
    std::tie(r1, c1, r2, c2) = v;

    size_t n = maze_1.size();

    unsigned levels[n][n][n][n];

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < n; ++k) {
                for (size_t l = 0; l < n; ++l) {
                    levels[i][j][k][l] = 0;
                }
            }
        }
    }

    levels[r1][c1][r2][c2] = 1;

    while (!q.empty()) {
        auto& u = q.front();
        std::tie(r1, c1, r2, c2) = u;

        for (unsigned int shift = 0; shift < 4; ++shift) { // for adjacent vertexes
            unsigned int r1_ = r1 + shifts[shift][0];
            unsigned int c1_ = c1 + shifts[shift][1];
            unsigned int r2_ = r2 + shifts[shift][0];
            unsigned int c2_ = c2 + shifts[shift][1];

            if (r1_ >= n) r1_ = r1;
            if (c1_ >= n) c1_ = c1;
            if (r2_ >= n) r2_ = r2;
            if (c2_ >= n) c2_ = c2;

            if (maze_1[r1_][c1_] == 1) {
                r1_ = r1;
                c1_ = c1;
            }
            if (maze_2[r2_][c2_] == 1) {
                r2_ = r2;
                c2_ = c2;
            }
            if (r1_ == r1 && c1_ == c1 && r2_ == r2 && c2_ == c2) {
                continue;
            }

            if (levels[r1_][c1_][r2_][c2_] == 0) {
                q.push(std::make_tuple(r1_, c1_, r2_, c2_));
                levels[r1_][c1_][r2_][c2_] = levels[r1][c1][r2][c2] + 1;
            }
        }
        q.pop();
    }

    return levels[0][0][0][0] - 1;
}


int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: ./a.out INPUT OUTPUT" << std::endl;
        return 1;
    }

    std::string input{argv[1]};
    std::string output{argv[2]};

    maze maze_1;
    maze maze_2;
    vertex v;
    std::tie(maze_1, maze_2, v) = parse_maze(input);

    int result = bfs(maze_1, maze_2, v);

    std::ofstream ofs(output);
    ofs << result << std::endl;

    return 0;
}

