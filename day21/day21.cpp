
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <limits>
#include "advent.hpp"

using key_map_t = std::unordered_map<char, xy_pos_t>;
using move_map_t = std::unordered_map<xy_pos_t, char>;

// clang-format off
key_map_t keys1 = {
    { '7', xy_pos_t{ 0, 0 } }, { '8', xy_pos_t{ 1, 0 } }, { '9', xy_pos_t{ 2, 0 } }, 
    { '4', xy_pos_t{ 0, 1 } }, { '5', xy_pos_t{ 1, 1 } }, { '6', xy_pos_t{ 2, 1 } }, 
    { '1', xy_pos_t{ 0, 2 } }, { '2', xy_pos_t{ 1, 2 } }, { '3', xy_pos_t{ 2, 2 } }, 
                               { '0', xy_pos_t{ 1, 3 } }, { 'A', xy_pos_t{ 2, 3 } }
};
key_map_t keys2 = {
                               { '^', xy_pos_t{ 1, 0 } }, { 'A', xy_pos_t{ 2, 0 } }, 
    { '<', xy_pos_t{ 0, 1 } }, { 'v', xy_pos_t{ 1, 1 } }, { '>', xy_pos_t{ 2, 1 } }
};
// clang-format on
move_map_t moves = { { xy_pos_t{ 1, 0 }, '>' },
                     { xy_pos_t{ 0, 1 }, 'v' },
                     { xy_pos_t{ -1, 0 }, '<' },
                     { xy_pos_t{ 0, -1 }, '^' },
                     { xy_pos_t{ 0, 0 }, 'A' } };

using char_row_t = std::vector<char>;
using codes_list_t = std::vector<char_row_t>;

codes_list_t read_file()
{
    std::ifstream infile("input.txt");
    codes_list_t codes_list;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        char_row_t row;

        char value{ 0 };
        while (ss >> value) {
            row.push_back(value);
        }

        codes_list.push_back(std::move(row));
    }

    return codes_list;
}

char_row_t try_press(xy_pos_t step1, xy_pos_t step2, xy_pos_t current, xy_pos_t desired_1, xy_pos_t desired_2,
                     xy_pos_t forbid)
{
    char_row_t test2;
    char_row_t test;
    while (step1 != xy_pos_t{ 0, 0 } && current != desired_1) {
        current = current + step1;
        test.push_back(moves[step1]);
        if (current == forbid)
            return test2;
    }
    while (step2 != xy_pos_t{ 0, 0 } && current != desired_2) {
        current = current + step2;
        test.push_back(moves[step2]);
        if (current == forbid)
            return test2;
    }
    test.push_back(moves[xy_pos_t{ 0, 0 }]);
    return test;
}

codes_list_t compute_press(key_map_t &keys, char key, xy_pos_t &current_pos, xy_pos_t forbid)
{
    codes_list_t out;

    xy_pos_t desired_pos = keys[key];
    xy_pos_t diff = desired_pos - current_pos;
    xy_pos_t x_step = diff.first == 0 ? xy_pos_t{ 0, 0 } : xy_pos_t{ diff.first / std::abs(diff.first), 0 };
    xy_pos_t y_step = diff.second == 0 ? xy_pos_t{ 0, 0 } : xy_pos_t{ 0, diff.second / std::abs(diff.second) };

    //2 options
    xy_pos_t step1 = x_step;
    xy_pos_t desired_1 = xy_pos_t{ desired_pos.first, current_pos.second };

    xy_pos_t step2 = y_step;
    xy_pos_t desired_2 = xy_pos_t{ current_pos.first, desired_pos.second };

    auto test = try_press(step1, step2, current_pos, desired_1, desired_pos, forbid);
    auto test2 = try_press(step2, step1, current_pos, desired_2, desired_pos, forbid);

    if (!test.empty())
        out.push_back(test);
    if (!test2.empty() && test2 != test)
        out.push_back(test2);

    return out;
}

codes_list_t compute_sequence2(key_map_t &keys, char_row_t codes, xy_pos_t current_pos, xy_pos_t forbid)
{
    std::vector<codes_list_t> out;

    for (auto key : codes) {
        auto test = compute_press(keys, key, current_pos, forbid);
        current_pos = keys[key];
        out.push_back(test);
    }

    codes_list_t decompress;
    size_t dim{ 1 };
    for (const auto &list : out)
        dim *= list.size();
    decompress.reserve(dim);
    std::vector<size_t> index(out.size(), 0);
    for (size_t i = 0; i < dim; i++) {
        decompress.emplace_back();
        for (size_t j = 0; j < out.size(); j++) {
            const auto &list = out.at(j);
            const auto &row = list.at(index.at(j));
            for (const auto &key : row)
                decompress.back().push_back(key);
            auto test = list.size();
        }
        //increment index
        for (size_t j = 0; j < out.size(); j++) {
            if (out.at(j).size() == index.at(j) + 1) {
                index.at(j) = 0;
                continue;
            } else {
                index.at(j)++;
                break;
            }
        }
    }

    return decompress;
}

int main()
{
    auto codes_list = read_file();

    int64_t complexity {0};

    std::vector<size_t> min_count(codes_list.size(), std::numeric_limits<size_t>::max());
    for (size_t i = 0; i < codes_list.size(); i++) {
        xy_pos_t start_pos = keys1['A'];

        auto out = compute_sequence2(keys1, codes_list.at(i), start_pos, xy_pos_t{ 0, 3 });

        xy_pos_t start_pos2 = keys2['A'];

        for (const auto &seq : out) {
            auto out2 = compute_sequence2(keys2, seq, start_pos2, xy_pos_t{ 0, 0 });
            for (const auto &seq2 : out2) {
                auto out3 = compute_sequence2(keys2, seq2, start_pos2, xy_pos_t{ 0, 0 });
                for (const auto & el : out3) {
                    if (el.size() < min_count.at(i))
                        min_count.at(i) = el.size();
                }
                std::cout << "HALP\n";
            }
        }

        int64_t num_code = (codes_list.at(i).at(0) - '0') * 100 + (codes_list.at(i).at(1) - '0') * 10 + (codes_list.at(i).at(2) - '0');
        complexity += num_code * min_count.at(i);
    }

    std::cout << complexity << '\n';
}