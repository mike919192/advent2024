
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <cmath>
#include <tuple>
#include <ranges>
#include <iostream>
#include <future>
#include <vector>
#include <unordered_map>

using num_type_t = long;
struct stone {
    num_type_t value{ 0 };
    int iteration{ 0 };
    bool operator==(const stone &) const = default;
};

template <>
struct std::hash<stone> {
    std::size_t operator()(const stone &k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return ((hash<num_type_t>()(k.value) ^ (hash<int>()(k.iteration) << 1)) >> 1);
    }
};

using stone_list_t = std::vector<stone>;

stone_list_t read_file()
{
    std::ifstream infile("input.txt");

    std::string line;
    std::getline(infile, line);
    std::istringstream ss(line);
    stone_list_t row;

    num_type_t num{ 0 };
    while (ss >> num) {
        row.push_back(stone{ .value = num });
    }

    return row;
}

bool is_even_num_digits(num_type_t num)
{
    //find the log10 of b
    double log_num = std::log10(static_cast<double>(num));
    //round it up
    double round_up = std::floor(log_num) + 1.0;
    //get the num digits
    num_type_t num_digits = std::lround(round_up);
    return num_digits % 2 == 0;
}

std::tuple<num_type_t, num_type_t> split_num(num_type_t num)
{
    //find the log10 of b
    double log_num = std::log10(static_cast<double>(num));
    //round it up
    double round_up = std::floor(log_num) + 1.0;
    //get the num digits
    double scale = std::pow(10, round_up / 2);
    long lscale = std::lround(scale);
    num_type_t num1 = num / lscale;
    num_type_t num2 = num % lscale;
    return { num1, num2 };
}

num_type_t stone_eval(stone a_stone, num_type_t max_iter, std::unordered_map<stone, num_type_t> &umap)
{
    //if we reached max_iter then remove it
    if (a_stone.iteration >= max_iter) {
        return 1;
    }
    //check if it exists in the map
    auto itr = umap.find(a_stone);
    if (itr != umap.end()) {
        return (*itr).second;
    }

    num_type_t result{ 0 };
    stone save_stone = a_stone;
    //if stone is 0 it becomes 1
    //if even number of digits it splits
    //else the stone is multiplied by 2024
    if (a_stone.value == 0) {
        a_stone.value = 1;
        a_stone.iteration++;
        result = stone_eval(a_stone, max_iter, umap);
    } else if (is_even_num_digits(a_stone.value)) {
        auto [num1, num2] = split_num(a_stone.value);
        a_stone.value = num1;
        a_stone.iteration++;
        stone new_stone = stone{ .value = num2, .iteration = a_stone.iteration };
        result = stone_eval(a_stone, max_iter, umap);
        result += stone_eval(new_stone, max_iter, umap);
    } else {
        a_stone.value *= 2024;
        a_stone.iteration++;
        result = stone_eval(a_stone, max_iter, umap);
    }
    umap[save_stone] = result;
    return result;
}

int main()
{
    auto stones_part = read_file();
    std::unordered_map<stone, num_type_t> umap;

    num_type_t sum_part1{ 0 };
    for (const auto &a_stone : stones_part) {
        sum_part1 += stone_eval(a_stone, 25, umap);
    }

    std::cout << sum_part1 << '\n';

    std::unordered_map<stone, num_type_t> umap2;

    num_type_t sum_part2{ 0 };
    for (const auto &a_stone : stones_part) {
        sum_part2 += stone_eval(a_stone, 75, umap2);
    }

    std::cout << sum_part2 << '\n';
}