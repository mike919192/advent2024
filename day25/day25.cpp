
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <iostream>

struct pins {
    std::array<int, 5> pin_heights{ 0 };
};

using pin_list_t = std::vector<pins>;

std::tuple<pin_list_t, pin_list_t> read_file()
{
    std::ifstream infile("input.txt");
    pin_list_t locks, keys;
    pins read_pins;
    bool is_lock{ false };
    size_t line_num{ 0 };

    for (std::string line; std::getline(infile, line);) {
        if (line.empty()) {
            if (is_lock)
                locks.push_back(read_pins);
            else
                keys.push_back(read_pins);
            read_pins = pins{ .pin_heights = { 0 } };
            line_num = 0;
            is_lock = false;
            continue;
        }
        std::istringstream ss(line);

        std::array<char, 5> values{ 0 };
        for (size_t i = 0; i < values.size(); i++) {
            ss >> values.at(i);
        }

        if (line_num == 0 && values.at(0) == '#') {
            is_lock = true;
        } else if (line_num < 6) {
            for (size_t i = 0; i < values.size(); i++) {
                if (values.at(i) == '#')
                    read_pins.pin_heights.at(i)++;
            }
        }
        line_num++;
    }

    if (is_lock)
        locks.push_back(read_pins);
    else
        keys.push_back(read_pins);

    return { locks, keys };
}

int main()
{
    auto [locks, keys] = read_file();

    int no_overlap_count{ 0 };

    for (const auto &lock : locks) {
        for (const auto &key : keys) {
            std::array<int, 5> overlap;
            for (size_t i = 0; i < overlap.size(); i++) {
                overlap.at(i) = lock.pin_heights.at(i) + key.pin_heights.at(i);
            }
            bool is_overlapping = std::any_of(overlap.begin(), overlap.end(), [](auto &a) { return a > 5; });
            if (!is_overlapping)
                no_overlap_count++;
        }
    }

    std::cout << no_overlap_count << '\n';
}