
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <ranges>
#include <cmath>

using int_row_t = std::vector<long>;
using int_rows_t = std::vector<int_row_t>;

std::tuple<int_row_t, int_row_t> read_file()
{
    std::ifstream infile("input.txt");
    int_row_t registers;
    int_row_t program;

    for (std::string line; std::getline(infile, line);) {
        auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos)
            break;
        std::string line_sub = line.substr(colon_pos + 1);
        std::istringstream ss(line_sub);

        long value{ 0 };
        ss >> value;
        registers.push_back(value);
    }

    std::string line;
    std::getline(infile, line);
    auto colon_pos = line.find(':');
    std::string line_sub = line.substr(colon_pos + 1);
    std::istringstream ss(line_sub);

    long value1{ 0 };
    long value2{ 0 };
    char comma{ 0 };
    ss >> value1 >> comma >> value2;
    program.push_back(value1);
    program.push_back(value2);

    while (ss >> comma >> value1 >> comma >> value2) {
        program.push_back(value1);
        program.push_back(value2);
    }

    return { registers, program };
}

long decode_combo(long operand, int_row_t &registers)
{
    switch (operand) {
    case 0 ... 3:
        return operand;
        break;
    case 4:
        return registers.at(0);
        break;
    case 5:
        return registers.at(1);
        break;
    case 6:
        return registers.at(2);
        break;
    default:
        throw std::runtime_error("Error decode logic!");
    }
}

long two_raised_to(long power)
{
    return std::lround(std::pow(2.0, power));
}

void decode_and_execute(long operate, long operand, int_row_t &registers, size_t &instr_ptr, int_row_t &output)
{
    switch (operate) {
    case 0: {
        long value = decode_combo(operand, registers);
        long result = two_raised_to(value);
        if (result == 0)
            output.push_back(-1);
        else
            registers.at(0) = registers.at(0) / result;
        instr_ptr++;
        break;
    }
    case 1: {
        long result = registers.at(1) ^ operand;
        registers.at(1) = result;
        instr_ptr++;
        break;
    }
    case 2: {
        long value = decode_combo(operand, registers);
        long result = value % 8;
        registers.at(1) = result;
        instr_ptr++;
        break;
    }
    case 3: {
        if (registers.at(0) == 0) {
            instr_ptr++;
        } else {
            instr_ptr = operand / 2;
        }
        break;
    }
    case 4: {
        long result = registers.at(1) ^ registers.at(2);
        registers.at(1) = result;
        instr_ptr++;
        break;
    }
    case 5: {
        long value = decode_combo(operand, registers);
        long result = value % 8;
        output.push_back(result);
        instr_ptr++;
        break;
    }
    case 6: {
        long value = decode_combo(operand, registers);
        long result = two_raised_to(value);
        if (result == 0)
            output.push_back(-1);
        else
            registers.at(1) = registers.at(0) / result;
        instr_ptr++;
        break;
    }
    case 7: {
        long value = decode_combo(operand, registers);
        long result = two_raised_to(value);
        if (result == 0)
            output.push_back(-1);
        else
            registers.at(2) = registers.at(0) / result;
        instr_ptr++;
        break;
    }
    default:
        throw std::runtime_error("Error decode logic!");
    }
}

int main()
{
    auto [registers, program] = read_file();
    int_row_t registers2 = registers;

    size_t instr_ptr{ 0 };
    int_row_t part1_output;

    while (2 * instr_ptr < program.size()) {
        long operate = program.at(2 * instr_ptr);
        long operand = program.at(2 * instr_ptr + 1);
        decode_and_execute(operate, operand, registers, instr_ptr, part1_output);
    }

    std::cout << part1_output.at(0);
    for (size_t i : std::views::iota(1u, part1_output.size()))
        std::cout << ',' << part1_output.at(i);

    std::cout << '\n';

    //part 2
    long a_register{ 0 };
    size_t size_match{ 2 };
    bool found{ false };
    while (size_match <= 16) {
        int_row_t try_registers = registers2;
        try_registers.at(0) = a_register;
        int_row_t part2_output;

        instr_ptr = 0;
        while (2 * instr_ptr < program.size()) {
            long operate = program.at(2 * instr_ptr);
            long operand = program.at(2 * instr_ptr + 1);
            decode_and_execute(operate, operand, try_registers, instr_ptr, part2_output);
            if (part2_output.size() >= size_match &&
                std::equal(part2_output.rbegin(), part2_output.rend(), program.rbegin()))
                found = true;
        }
        if (found) {
            std::cout << part2_output.at(0);
            for (size_t i : std::views::iota(1u, part2_output.size()))
                std::cout << ',' << part2_output.at(i);

            std::cout << '\n';
            std::cout << a_register << '\n';
            size_match += 2;
            a_register = a_register << 2l * 3l;
        } else {
            a_register++;
        }
        found = false;
    }
}