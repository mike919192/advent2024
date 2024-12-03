
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

std::vector<int> computeDiff(const std::vector<int> &in)
{
    std::vector<int> diff(in.size() - 1);

    for (size_t i = 1; i < in.size(); i++) {
        diff.at(i - 1) = in.at(i) - in.at(i - 1);
    }

    return diff;
}

bool isSafe(const std::vector<int> &in)
{
    std::vector<int> diff = computeDiff(in);

    auto allpos = [](auto a) { return a > 0; };
    auto allneg = [](auto a) { return a < 0; };
    auto smalldiff = [](auto a) { return std::abs(a) <= 3; };

    bool isallpos = std::all_of(diff.begin(), diff.end(), allpos);
    bool isallneg = std::all_of(diff.begin(), diff.end(), allneg);
    bool issmalldiff = std::all_of(diff.begin(), diff.end(), smalldiff);

    return (isallpos || isallneg) && issmalldiff;
}

int main()
{
    std::ifstream infile("input.txt");
    std::vector<std::vector<int>> csvRows;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(std::move(line));
        std::vector<int> row;

        int value;
        while (ss >> value) {
            row.push_back(value);
        }

        csvRows.push_back(std::move(row));
    }

    int safereport{ 0 };

    for (auto const &row : csvRows) {
        bool unmodifiedsafe = isSafe(row);
        bool modifiedsafe{ false };

        if (unmodifiedsafe == false) {
            for (size_t i = 0; i < row.size(); i++) {
                std::vector<int> mod_row = row;
                mod_row.erase(std::next(mod_row.begin(), i));

                modifiedsafe |= isSafe(mod_row);
                if (modifiedsafe)
                    break;
            }
        }

        if (unmodifiedsafe || modifiedsafe) {
            safereport++;
        }
    }

    std::cout << safereport << "\n";
}