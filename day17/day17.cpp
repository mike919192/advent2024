
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <iostream>

using int_row_t = std::vector<long>;
using int_rows_t = std::vector<int_row_t>;

std::tuple<int_row_t, int_rows_t> read_file()
{
    std::ifstream infile("input.txt");
    int_row_t registers;
    int_rows_t program;

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
    program.emplace_back();
    program.back().push_back(value1);
    program.back().push_back(value2);

    while (ss >> comma >> value1 >> comma >> value2) {
        program.emplace_back();
        program.back().push_back(value1);
        program.back().push_back(value2);
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
    return 2 << (power - 1);
}

void decode_and_execute(long operate, long operand, int_row_t &registers, size_t &instr_ptr)
{
    switch (operate) {
    case 0: {
        long value = decode_combo(operand, registers);
        long result = two_raised_to(value);
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
        std::cout << result << ',';
        instr_ptr++;
        break;
    }
    case 6: {
        long value = decode_combo(operand, registers);
        long result = two_raised_to(value);
        registers.at(1) = registers.at(0) / result;
        instr_ptr++;
        break;
    }
    case 7: {
        long value = decode_combo(operand, registers);
        long result = two_raised_to(value);
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

    size_t instr_ptr{ 0 };

    while (instr_ptr < program.size()) {
        long operate = program.at(instr_ptr).at(0);
        long operand = program.at(instr_ptr).at(1);
        decode_and_execute(operate, operand, registers, instr_ptr);
    }
}