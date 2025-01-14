
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

        int64_t value {0};
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

    for (size_t i : std::views::iota(0u, 2000u))
    {
        for (size_t j : std::views::iota(0u, secrets.size()))
            secrets.at(j) = evolve_secret(secrets.at(j));
    }

    int64_t sum = std::accumulate(secrets.begin(), secrets.end(), 0ll);

    std::cout << sum << '\n';
}