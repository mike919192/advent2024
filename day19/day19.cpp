
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

bool remake_design(const std::string_view design, const string_row_t &towels, const std::vector<size_t> &contains_index)
{
    for (size_t i = 0; i < contains_index.size(); i++) {
        std::string_view towel = towels.at(contains_index.at(i));
        if (design.starts_with(towel)) {
            if (design.size() == towel.size()) {
                //we are done with the design
                return true;
            } else {
                //new subview
                std::string_view sub_design = design.substr(towel.size());
                bool check = remake_design(sub_design, towels, contains_index);
                if (check)
                    return check;
            }
        }
    }

    return false;
}

long remake_design_part2(const std::string_view design, const string_row_t &towels,
                         const std::vector<size_t> &contains_index, std::unordered_map<std::string, long> &umap)
{
    long result{ 0 };
    for (size_t i = 0; i < contains_index.size(); i++) {
        std::string_view towel = towels.at(contains_index.at(i));
        if (design.starts_with(towel)) {
            if (design.size() == towel.size()) {
                //we are done with the design
                result++;
            } else {
                //new subview
                std::string_view sub_design = design.substr(towel.size());
                if (umap.contains(std::string(sub_design))) {
                    result += umap[std::string(sub_design)];
                } else {
                    long temp_result = remake_design_part2(sub_design, towels, contains_index, umap);
                    umap[std::string(sub_design)] = temp_result;
                    result += temp_result;
                }
            }
        }
    }

    return result;
}

int main()
{
    auto [towels, designs] = read_file();

    long result{ 0 };
    long result_part2{ 0 };
    std::unordered_map<std::string, long> umap;

    for (const auto &design : designs) {
        std::vector<size_t> contains_index;

        for (size_t i = 0; i < towels.size(); i++) {
            if (string_contains(design, towels.at(i)))
                contains_index.push_back(i);
        }

        if (remake_design(design, towels, contains_index))
            result++;

        if (umap.contains(design)) {
            result_part2 += umap[design];
        } else {
            long temp_result = remake_design_part2(design, towels, contains_index, umap);
            umap[design] = temp_result;
            result_part2 += temp_result;
        }
    }

    std::cout << result << '\n';
    std::cout << result_part2 << '\n';
}