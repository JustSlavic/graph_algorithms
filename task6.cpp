//
// Created by radko on 6/30/17.
//

#include <fstream>
#include <vector>

using namespace std;

unsigned read_data(string filename, vector<int>& stct_x, vector<int>& stct_y, vector<int>& power_x, vector<int>& power_y) {
    ifstream ifs(filename);

    unsigned n, k;

    ifs >> n >> k;

    int tmp;

    for (int i = 0; i < k; ++i) {
        ifs >> tmp;
        stct_x.emplace_back(tmp);
    }

    for (int j = 0; j < k; ++j) {
        ifs >> tmp;
        stct_y.emplace_back(tmp);
    }

    for (int i = 0; i < k; ++i) {
        ifs >> tmp;
        power_x.emplace_back(tmp);
    }

    for (int j = 0; j < k; ++j) {
        ifs >> tmp;
        power_y.emplace_back(tmp);
    }

    ifs.close();

    return n;
}

int main() {

    vector<int> stct_x, stct_y, power_x, power_y; // stct = space and time continuum transgression

    unsigned n = read_data("input.txt", stct_x, stct_y, power_x, power_y);

    return EXIT_SUCCESS;
}