
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
const key_map_t keys_numpad = {
    { '7', xy_pos_t{ 0, 0 } }, { '8', xy_pos_t{ 1, 0 } }, { '9', xy_pos_t{ 2, 0 } }, 
    { '4', xy_pos_t{ 0, 1 } }, { '5', xy_pos_t{ 1, 1 } }, { '6', xy_pos_t{ 2, 1 } }, 
    { '1', xy_pos_t{ 0, 2 } }, { '2', xy_pos_t{ 1, 2 } }, { '3', xy_pos_t{ 2, 2 } }, 
                               { '0', xy_pos_t{ 1, 3 } }, { 'A', xy_pos_t{ 2, 3 } }
};
const key_map_t keys_dir = {
                               { '^', xy_pos_t{ 1, 0 } }, { 'A', xy_pos_t{ 2, 0 } }, 
    { '<', xy_pos_t{ 0, 1 } }, { 'v', xy_pos_t{ 1, 1 } }, { '>', xy_pos_t{ 2, 1 } }
};
// clang-format on
const move_map_t move_map = { { xy_pos_t{ 1, 0 }, '>' },
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

        return hash<char>{}(k.key) ^ (hash<xy_pos_t>{}(k.pos) << 1u) ^ (hash<int>{}(k.iter) << 2u);
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
        moves_vector.push_back(move_map.at(step1));
        if (current == forbid)
            return char_row_t{};
    }
    while (step2 != xy_pos_t{ 0, 0 } && current != desired_2) {
        current = current + step2;
        moves_vector.push_back(move_map.at(step2));
        if (current == forbid)
            return char_row_t{};
    }
    moves_vector.push_back(move_map.at(xy_pos_t{ 0, 0 }));
    return moves_vector;
}

//this returns either 1 or 2 possible moves to get to the desired key
//for example <<^A or ^<<A
//<^<A is never omptimal so it is never considered
codes_list_t compute_all_moves(const key_map_t &keys, char key, xy_pos_t &current_pos, xy_pos_t forbid)
{
    codes_list_t all_moves;

    xy_pos_t desired_pos = keys.at(key);
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

template <bool first_iter_t = false>
size_t compute_sequence(const key_map_t &keys1, const key_map_t &keys2, const char_row_t &codes, xy_pos_t forbid1,
                        xy_pos_t forbid2, int iter, int max_iter, memo_map_t &memo_map)
{
    const key_map_t &keys_to_use = first_iter_t ? keys1 : keys2;
    xy_pos_t forbid_to_use = first_iter_t ? forbid1 : forbid2;
    xy_pos_t current_pos = keys_to_use.at('A');
    size_t total_size{ 0 };
    for (auto key : codes) {
        size_t moves_size{ 0 };
        memo_inputs keyvalue{ .key = key, .pos = current_pos, .iter = iter };
        if (memo_map.contains(keyvalue)) {
            moves_size = memo_map[keyvalue];
            current_pos = keys_to_use.at(key);
        } else {
            auto all_moves = compute_all_moves(keys_to_use, key, current_pos, forbid_to_use);

            if (iter < max_iter) {
                std::array<size_t, 2> values{ std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max() };
                size_t i{ 0 };
                for (const auto &seq : all_moves) {
                    values.at(i) = compute_sequence(keys1, keys2, seq, forbid1, forbid2, iter + 1, max_iter, memo_map);
                    i++;
                }
                moves_size += std::min(values.at(0), values.at(1));
            } else {
                if (all_moves.size() == 1)
                    moves_size += all_moves.at(0).size();
                else
                    moves_size += std::min(all_moves.at(0).size(), all_moves.at(1).size());
            }
            memo_map[keyvalue] = moves_size;
        }
        total_size += moves_size;
    }

    return total_size;
}

int main()
{
    auto codes_list = read_file();

    int64_t complexity_part1{ 0 };
    int64_t complexity_part2{ 0 };
    memo_map_t memo_map_part1;
    memo_map_t memo_map_part2;

    for (const auto &codes : codes_list) {
        auto num_moves_part1 = compute_sequence<true>(keys_numpad, keys_dir, codes, xy_pos_t{ 0, 3 }, xy_pos_t{ 0, 0 },
                                                      0, 2, memo_map_part1);

        auto num_moves_part2 = compute_sequence<true>(keys_numpad, keys_dir, codes, xy_pos_t{ 0, 3 }, xy_pos_t{ 0, 0 },
                                                      0, 25, memo_map_part2);

        int64_t num_code = (codes.at(0) - '0') * 100 + (codes.at(1) - '0') * 10 + (codes.at(2) - '0');
        complexity_part1 += num_code * num_moves_part1;
        complexity_part2 += num_code * num_moves_part2;
    }

    std::cout << complexity_part1 << '\n';
    std::cout << complexity_part2 << '\n';
}