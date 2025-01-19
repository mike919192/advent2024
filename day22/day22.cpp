
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ranges>
#include <iostream>
#include <numeric>
#include <span>
#include <unordered_map>
#include "advent.hpp"

using secret_list_t = std::vector<int64_t>;

secret_list_t read_file()
{
    std::ifstream infile("input.txt");
    secret_list_t codes_list;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);

        int64_t value{ 0 };
        ss >> value;

        codes_list.push_back(value);
    }

    return codes_list;
}

int64_t mix(int64_t secret, int64_t value)
{
    return secret ^ value;
}

int64_t prune(int64_t secret)
{
    return secret % 16777216ll;
}

int64_t evolve_secret(int64_t secret)
{
    int64_t result1 = secret * 64;
    secret = mix(secret, result1);
    secret = prune(secret);

    int64_t result2 = secret / 32;
    secret = mix(secret, result2);
    secret = prune(secret);

    int64_t result3 = secret * 2048;
    secret = mix(secret, result3);
    secret = prune(secret);

    return secret;
}

template <typename t_t, size_t n_t>
int64_t encode_key(std::span<const t_t, n_t> diffs)
{
    int64_t encode_value{ 0 };
    //diffs use 5 bits for range of -16 to 15
    for (size_t i : std::views::iota(0u, diffs.size())) {
        encode_value |= (diffs[i] & 0x1F) << (5u * i);
    }

    return encode_value;
}

int main()
{
    auto secrets = read_file();
    auto secrets_part2_init = secrets;

    for (size_t j : std::views::iota(0u, secrets.size())) {
        for (size_t i : std::views::iota(0u, 2000u))
            secrets.at(j) = evolve_secret(secrets.at(j));
    }

    int64_t sum = std::accumulate(secrets.begin(), secrets.end(), 0ll);

    std::cout << sum << '\n';

    std::vector<std::vector<int64_t>> all_diffs;
    std::vector<std::vector<int64_t>> all_evolves;
    std::unordered_map<int64_t, int64_t> banana_map;

    //get all the diffs and the evolved values
    for (size_t j : std::views::iota(0u, secrets_part2_init.size())) {
        all_diffs.emplace_back();
        all_evolves.emplace_back();
        int64_t last_secret{ secrets_part2_init.at(j) };
        for (size_t i : std::views::iota(0u, 2000u)) {
            int64_t new_evolve = evolve_secret(last_secret);
            int diff = static_cast<int>((new_evolve % 10) - (last_secret % 10));
            all_diffs.back().push_back(diff);
            all_evolves.back().push_back(new_evolve);
            last_secret = new_evolve;
        }
    }

    //fill up a map with the encoded values
    for (size_t i : std::views::iota(0u, all_diffs.size())) {
        const auto &evolve = all_diffs.at(i);
        std::unordered_map<int64_t, int64_t> seller_map;
        for (size_t j : std::views::iota(0u, evolve.size() - 4)) {
            auto diff_span = std::span(std::next(evolve.begin(), j), std::next(evolve.begin(), j + 4));
            int64_t encoded_value = encode_key(diff_span);
            if (!seller_map.contains(encoded_value)) {
                seller_map[encoded_value] = all_evolves.at(i).at(j + 3) % 10;
            }
        }
        //merge the maps, duplicates get added
        for (const auto & el : seller_map) {
            if (!banana_map.contains(el.first)) {
                banana_map[el.first] = el.second;
            } else {
                banana_map[el.first] += el.second;
            }
        }
    }

    //find the max value in the map
    auto max_element = std::max_element(banana_map.begin(), banana_map.end(), [](auto a, auto b)
    {
        return a.second < b.second;
    });

    auto max_sum_part2 = max_element->second;
    
    std::cout << max_sum_part2 << '\n';
}