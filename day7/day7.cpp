
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <cmath>

using num_type = long;

std::vector<std::vector<num_type>> read_file()
{
    std::ifstream infile("input.txt");
    std::vector<std::vector<num_type>> csv_rows;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        std::vector<num_type> row;

        num_type result{ 0 };
        char colon{ 0 };
        num_type operand{ 0 };
        ss >> result >> colon;
        row.push_back(result);
        while (ss >> operand) {
            row.push_back(operand);
        }

        csv_rows.push_back(std::move(row));
    }

    return csv_rows;
}

enum class oper : unsigned int { add = 0, multiply = 1, concat = 2 };

num_type do_oper(oper oper_in, num_type a, num_type b)
{
    switch (oper_in) {
    case oper::add:
        return a + b;
        break;

    case oper::multiply:
        return a * b;
        break;

    case oper::concat: {
        //find the log10 of b
        double log_b = std::log10(static_cast<double>(b));
        //round it up
        //double round_up = std::ceil(log_b);
        double round_up = std::floor(log_b) + 1.0;
        //compute the scale for a
        double scale = std::pow(10, round_up);
        num_type lscale = std::lround(scale);
        num_type value = a * lscale + b;
        return value;
    } break;

    default:
        throw std::runtime_error("Error doing operation!");
        break;
    }
}

struct oper_permutator {
    unsigned int num_of_operators{ 0 };
    std::array<oper, 32> opers{};

    //return false when the permutation repeats
    bool next_permutation()
    {
        for (size_t i = 0; i < opers.size(); i++) {
            if (opers.at(i) == oper::add) {
                opers.at(i) = oper::multiply;
                break;
            } else if (opers.at(i) == oper::multiply) {
                opers.at(i) = oper::concat;
                //opers.at(i) = oper::add;
                break;
            } else if (opers.at(i) == oper::concat) {
                opers.at(i) = oper::add;
            } else {
                throw std::runtime_error("Error doing permutation!");
            }
        }
        return !std::all_of(opers.begin(), std::next(opers.begin(), num_of_operators),
                            [](oper i) { return i == oper::add; });
    }

    oper get_oper(unsigned int position)
    {
        return opers.at(position);
    }
};

int main()
{
    const auto equations = read_file();

    num_type result_sum{ 0 };

    for (const auto &equation : equations) {
        oper_permutator permu{ .num_of_operators = equation.size() - 2 };

        //std::cout << "halp\n";

        do {
            num_type result = equation.at(1);
            for (size_t i = 2; i < equation.size(); i++) {
                oper oper_to_do = permu.get_oper(i - 2);
                result = do_oper(oper_to_do, result, equation.at(i));
                if (result < 0)
                    std::cout << "here\n";
            }
            if (result == equation.at(0)) {
                result_sum += result;
                break;
            }
        } while (permu.next_permutation());

        //std::cout << "halp\n";
    }

    std::cout << result_sum << '\n';
}