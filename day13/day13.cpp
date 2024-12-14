
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <iostream>

using int_row_t = std::vector<long>;
using int_rows_t = std::vector<int_row_t>;

int_rows_t read_file()
{
    std::ifstream infile("input.txt");
    int_rows_t csv_rows;

    for (std::string line; std::getline(infile, line);) {
        auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos)
            continue;
        std::string line_sub = line.substr(colon_pos + 1);
        std::istringstream ss(line_sub);
        int_row_t row;

        char label{ 0 };
        long value1{ 0 };
        long value2{ 0 };
        ss >> label >> label >> value1 >> label >> label >> label >> value2;
        row.push_back(value1);
        row.push_back(value2);

        csv_rows.push_back(std::move(row));
    }

    return csv_rows;
}

bool num_within_abs(double num1, double num2, double tolerance)
{
    return std::abs(num1 - num2) <= tolerance;
}

long solve_eqs(const int_rows_t &eqs, long prize_offset)
{
    long token_sum{ 0 };
    for (size_t i = 0; i < eqs.size() / 3; i++) {
        long a = eqs.at(i * 3).at(0);
        long b = eqs.at(i * 3 + 1).at(0);
        long c = eqs.at(i * 3).at(1);
        long d = eqs.at(i * 3 + 1).at(1);

        double scale = 1.0 / (a * d - b * c);
        double a_inv = scale * d;
        double b_inv = scale * (-b);
        double c_inv = scale * (-c);
        double d_inv = scale * a;

        long prize_x = eqs.at(i * 3 + 2).at(0) + prize_offset;
        long prize_y = eqs.at(i * 3 + 2).at(1) + prize_offset;

        double a_press = a_inv * prize_x + b_inv * prize_y;
        double b_press = c_inv * prize_x + d_inv * prize_y;

        double a_press_rnd = std::round(a_press);
        double b_press_rnd = std::round(b_press);

        if (num_within_abs(a_press, a_press_rnd, 1e-3) && num_within_abs(b_press, b_press_rnd, 1e-3)) {
            token_sum += std::lround(a_press) * 3 + std::lround(b_press);
        }
    }

    return token_sum;
}

int main()
{
    const auto values = read_file();

    long token_sum_part1 = solve_eqs(values, 0);
    long token_sum_part2 = solve_eqs(values, 10000000000000l);

    std::cout << token_sum_part1 << '\n';
    std::cout << token_sum_part2 << '\n';
}