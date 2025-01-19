
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ranges>
#include <iostream>
#include <numeric>

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

    std::array<int, 4> changes{ -9,-9,-9,-9 };
    int64_t max_sum_part2{ 0 };
    std::vector<std::vector<int64_t>> all_diffs;
    std::vector<std::vector<int64_t>> all_evolves;

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
    //int i {0};

    do {
        int64_t sum_part2{ 0 };
        //std::cout << "Iter" << i << '\n';
        //i++;

        for (size_t i : std::views::iota(0u, all_diffs.size())) {
            const auto &evolve = all_diffs.at(i);
            for (size_t j : std::views::iota(0u, evolve.size() - 4)) {
                if (evolve.at(j) == changes.at(0) && evolve.at(j + 1) == changes.at(1) &&
                    evolve.at(j + 2) == changes.at(2) && evolve.at(j + 3) == changes.at(3)) {
                    int64_t test = all_evolves.at(i).at(j + 3) % 10;
                    sum_part2 += test;
                    break;
                }
            }
        }
        for (size_t i : std::views::iota(0u, 4u)) {
            if (changes.at(i) == 9) {
                changes.at(i) = -9;
            } else {
                changes.at(i)++;
                break;
            }
        }
        if (sum_part2 > max_sum_part2)
            max_sum_part2 = sum_part2;
    } while (changes != std::array<int, 4>{ 9, 9, 9, 9 });
    std::cout << max_sum_part2 << '\n';
}