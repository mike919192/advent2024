
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "advent.hpp"

using string_row_t = std::vector<std::string>;

std::tuple<string_row_t, string_row_t> read_file()
{
    std::ifstream infile("input.txt");
    string_row_t towels;
    string_row_t designs;

    {
        std::string line;
        std::getline(infile, line);
        std::istringstream ss(line);

        std::string value;

        while (getline(ss, value, ',')) {
            ltrim(value);
            towels.push_back(value);
        }

        //blank line
        std::getline(infile, line);
    }

    for (std::string line; std::getline(infile, line);)
        designs.push_back(line);

    return { towels, designs };
}

long build_design(const std::string_view design, const string_row_t &towels, const std::vector<size_t> &contains_index,
                  std::unordered_map<std::string, long> &umap)
{
    if (umap.contains(std::string(design))) {
        return umap[std::string(design)];
    }

    long result{ 0 };
    for (const auto i : contains_index) {
        std::string_view towel = towels.at(i);
        if (design.starts_with(towel)) {
            if (design.size() == towel.size()) {
                //we are done with the design
                result++;
            } else {
                //new subview
                std::string_view sub_design = design.substr(towel.size());
                result += build_design(sub_design, towels, contains_index, umap);
            }
        }
    }

    umap[std::string(design)] = result;

    return result;
}

int main()
{
    auto [towels, designs] = read_file();

    long result_part1{ 0 };
    long result_part2{ 0 };
    std::unordered_map<std::string, long> umap;

    for (const auto &design : designs) {
        std::vector<size_t> contains_index;

        size_t i{ 0 };
        for (const auto &towel : towels) {
            if (string_contains(design, towel))
                contains_index.push_back(i);
            i++;
        }

        long temp_result = build_design(design, towels, contains_index, umap);
        if (temp_result > 0) {
            result_part1++;
            result_part2 += temp_result;
        }
    }

    std::cout << result_part1 << '\n';
    std::cout << result_part2 << '\n';
}