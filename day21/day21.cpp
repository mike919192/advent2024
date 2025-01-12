
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <limits>
#include <array>
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

struct memo_inputs {
    char key{ 0 };
    xy_pos_t pos{ 0, 0 };
    int iter{ 0 };
    bool operator==(const memo_inputs &) const = default;
};

template <>
struct std::hash<memo_inputs> {
    size_t operator()(const memo_inputs &k) const
    {
        // Compute individual hash values for fields
        // and combine them using XOR
        // and bit shifting:

        return hash<char>{}(k.key) ^ (hash<xy_pos_t>{}(k.pos) << 1) ^ (hash<int>{}(k.iter) << 2);
    }
};

using memo_map_t = std::unordered_map<memo_inputs, int64_t>;

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

//this returns the sequence of moves to go from current -> desired_1 -> desired_2
//the order of the moves is step1 and then step2 (X then Y or Y then X)
//forbid is the empty space that we are not allowed to move to
//if we enter forbid then return vector is empty
char_row_t compute_moves(xy_pos_t step1, xy_pos_t step2, xy_pos_t current, xy_pos_t desired_1, xy_pos_t desired_2,
                         xy_pos_t forbid)
{
    char_row_t moves_vector;
    while (step1 != xy_pos_t{ 0, 0 } && current != desired_1) {
        current = current + step1;
        moves_vector.push_back(moves[step1]);
        if (current == forbid)
            return char_row_t{};
    }
    while (step2 != xy_pos_t{ 0, 0 } && current != desired_2) {
        current = current + step2;
        moves_vector.push_back(moves[step2]);
        if (current == forbid)
            return char_row_t{};
    }
    moves_vector.push_back(moves[xy_pos_t{ 0, 0 }]);
    return moves_vector;
}

//this returns either 1 or 2 possible moves to get to the desired key
//for example <<^A or ^<<A
//<^<A is never omptimal so it is never considered
codes_list_t compute_all_moves(key_map_t &keys, char key, xy_pos_t &current_pos, xy_pos_t forbid)
{
    codes_list_t all_moves;

    xy_pos_t desired_pos = keys[key];
    xy_pos_t diff = desired_pos - current_pos;
    xy_pos_t x_step = diff.first == 0 ? xy_pos_t{ 0, 0 } : xy_pos_t{ diff.first / std::abs(diff.first), 0 };
    xy_pos_t y_step = diff.second == 0 ? xy_pos_t{ 0, 0 } : xy_pos_t{ 0, diff.second / std::abs(diff.second) };

    //2 options
    xy_pos_t step1 = x_step;
    xy_pos_t desired_1 = xy_pos_t{ desired_pos.first, current_pos.second };

    xy_pos_t step2 = y_step;
    xy_pos_t desired_2 = xy_pos_t{ current_pos.first, desired_pos.second };

    auto move1 = compute_moves(step1, step2, current_pos, desired_1, desired_pos, forbid);
    auto move2 = compute_moves(step2, step1, current_pos, desired_2, desired_pos, forbid);

    if (!move1.empty())
        all_moves.push_back(move1);
    if (!move2.empty() && move2 != move1)
        all_moves.push_back(move2);

    current_pos = desired_pos;

    return all_moves;
}

size_t compute_sequence3(key_map_t &keys, char_row_t codes, xy_pos_t current_pos, xy_pos_t forbid, int &iter,
                         int max_iter, memo_map_t &memo_map)
{
    current_pos = keys['A'];
    size_t value{ 0 };
    for (auto key : codes) {
        size_t value2{ 0 };
        memo_inputs keyvalue{ .key = key, .pos = current_pos, .iter = iter };
        if (memo_map.contains(keyvalue)) {
            value2 = memo_map[keyvalue];
            current_pos = keys[key];
        } else {
            auto test = compute_all_moves(keys, key, current_pos, forbid);

            if (iter < max_iter) {
                std::array<int64_t, 2> values{ std::numeric_limits<int64_t>::max(),
                                               std::numeric_limits<int64_t>::max() };
                size_t i{ 0 };
                for (const auto &seq : test) {
                    int iter2 = iter + 1;
                    values.at(i) = compute_sequence3(keys, seq, current_pos, forbid, iter2, max_iter, memo_map);
                    i++;
                }
                value2 += std::min(values.at(0), values.at(1));
            } else {
                if (test.size() == 1)
                    value2 += test.at(0).size();
                else
                    value2 += std::min(test.at(0).size(), test.at(1).size());
            }
            memo_map[keyvalue] = value2;
        }
        value += value2;
    }

    return value;
}

size_t compute_sequence4(key_map_t &keys_one, key_map_t &keys_two, char_row_t codes, xy_pos_t current_pos,
                         xy_pos_t forbid, xy_pos_t forbid2, int &iter, int max_iter, memo_map_t &memo_map)
{
    current_pos = keys_one['A'];
    size_t value{ 0 };
    for (auto key : codes) {
        size_t value2{ 0 };
        memo_inputs keyvalue{ .key = key, .pos = current_pos, .iter = iter };
        if (memo_map.contains(keyvalue)) {
            value2 = memo_map[keyvalue];
            current_pos = keys_one[key];
        } else {
            auto test = compute_all_moves(keys_one, key, current_pos, forbid);

            if (iter < max_iter) {
                std::array<int64_t, 2> values{ std::numeric_limits<int64_t>::max(),
                                               std::numeric_limits<int64_t>::max() };
                size_t i{ 0 };
                for (const auto &seq : test) {
                    int iter2 = iter + 1;
                    values.at(i) = compute_sequence3(keys_two, seq, current_pos, forbid2, iter2, max_iter, memo_map);
                    i++;
                }
                value2 += std::min(values.at(0), values.at(1));
            } else {
                if (test.size() == 1)
                    value2 += test.at(0).size();
                else
                    value2 += std::min(test.at(0).size(), test.at(1).size());
            }
            memo_map[keyvalue] = value2;
        }
        value += value2;
    }

    return value;
}

int main()
{
    auto codes_list = read_file();

    int64_t complexity{ 0 };
    memo_map_t memo_map;
    memo_map_t memo_map2;

    for (size_t i = 0; i < codes_list.size(); i++) {
        xy_pos_t start_pos = keys1['A'];
        int iter2{ 0 };

        auto out2 = compute_sequence4(keys1, keys2, codes_list.at(i), start_pos, xy_pos_t{ 0, 3 }, xy_pos_t{ 0, 0 },
                                      iter2, 25, memo_map2);

        int64_t num_code =
            (codes_list.at(i).at(0) - '0') * 100 + (codes_list.at(i).at(1) - '0') * 10 + (codes_list.at(i).at(2) - '0');
        complexity += num_code * out2;
    }

    std::cout << complexity << '\n';
}