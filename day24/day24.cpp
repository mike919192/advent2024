
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <format>
#include <iostream>
#include <array>
#include <ranges>

enum class gate_operation { gate_and, gate_or, gate_xor };

const char *to_string(gate_operation v)
{
    switch (v) {
    case gate_operation::gate_and:
        return "AND";
    case gate_operation::gate_or:
        return "OR";
    case gate_operation::gate_xor:
        return "XOR";
    default:
        throw std::runtime_error("Error doing gate op!");
    }
}

bool do_gate_op(bool in1, bool in2, gate_operation op)
{
    if (op == gate_operation::gate_and)
        return in1 && in2;
    else if (op == gate_operation::gate_or)
        return in1 || in2;
    else if (op == gate_operation::gate_xor)
        return in1 ^ in2;
    else
        throw std::runtime_error("Error doing gate op!");
}

struct gate {
    std::string in_name1;
    std::string in_name2;
    gate_operation op{ gate_operation::gate_and };
    std::string out_name;
};

using wire_map_t = std::unordered_map<std::string, bool>;
using gate_list_t = std::vector<gate>;

std::tuple<wire_map_t, gate_list_t> read_file()
{
    std::ifstream infile("input.txt");
    wire_map_t wire_map;
    gate_list_t gate_list;

    for (std::string line; std::getline(infile, line);) {
        if (line.empty())
            break;
        std::istringstream ss(line);
        std::string name;
        int value{ 0 };

        getline(ss, name, ':');
        ss >> value;

        wire_map[name] = value != 0;
    }

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        std::array<std::string, 2> in_name;
        std::string op_str;
        std::string arrow;
        std::string out_name;
        gate_operation op{ gate_operation::gate_and };

        ss >> in_name.at(0) >> op_str >> in_name.at(1) >> arrow >> out_name;
        std::sort(in_name.begin(), in_name.end());

        if (op_str == "AND")
            op = gate_operation::gate_and;
        else if (op_str == "OR")
            op = gate_operation::gate_or;
        else if (op_str == "XOR")
            op = gate_operation::gate_xor;
        else
            throw std::runtime_error("Error parsing gate op!");

        gate_list.push_back(
            gate{ .in_name1 = in_name.at(0), .in_name2 = in_name.at(1), .op = op, .out_name = out_name });
    }

    return { wire_map, gate_list };
}

bool sim_tick(wire_map_t &wires, gate_list_t &gates, int &iter)
{
    if (iter > 100)
        return false;

    iter++;
    auto do_gate = [&wires](gate &a) {
        if (wires.contains(a.in_name1) && wires.contains(a.in_name2)) {
            bool out = do_gate_op(wires[a.in_name1], wires[a.in_name2], a.op);
            wires[a.out_name] = out;
        }
    };

    auto any_gates_todo = [&wires](gate &a) { return !wires.contains(a.out_name); };

    std::for_each(gates.begin(), gates.end(), do_gate);

    return std::any_of(gates.begin(), gates.end(), any_gates_todo);
}

int64_t get_outvalue(wire_map_t &wire_map)
{
    uint i{ 0 };
    std::string z_out_name = std::format("z{:02}", i);
    int64_t out_value{ 0 };

    while (wire_map.contains(z_out_name)) {
        int64_t value = wire_map[z_out_name];
        out_value |= value << i;
        i++;
        z_out_name = std::format("z{:02}", i);
    }

    return out_value;
}

wire_map_t create_wiremap(size_t bitsize, int64_t in1, int64_t in2)
{
    wire_map_t wiremap;

    for (size_t x = 0; x < bitsize; x++) {
        std::string in_name = std::format("x{:02}", x);
        wiremap[in_name] = ((in1 >> x) & 1) != 0;
    }

    for (size_t y = 0; y < bitsize; y++) {
        std::string in_name = std::format("y{:02}", y);
        wiremap[in_name] = ((in2 >> y) & 1) != 0;
    }

    return wiremap;
}

std::tuple<std::string, std::string, gate_operation> print_gate(gate_list_t &gates, std::string &out_name)
{
    auto itr = std::find_if(gates.begin(), gates.end(), [&out_name](gate &a) { return a.out_name == out_name; });

    std::cout << itr->in_name1 << " " << to_string(itr->op) << " " << itr->in_name2 << " = " << itr->out_name << '\n';

    return { itr->in_name1, itr->in_name2, itr->op };
}

std::tuple<std::string, std::string, gate_operation> get_inputs(gate_list_t &gates, std::string &out_name)
{
    auto itr = std::find_if(gates.begin(), gates.end(), [&out_name](gate &a) { return a.out_name == out_name; });

    //std::cout << itr->in_name1 << " " << to_string(itr->op) << " " << itr->in_name2 << " = " << itr->out_name << '\n';

    return { itr->in_name1, itr->in_name2, itr->op };
}

void repair1(gate_list_t &gates, size_t i, std::vector<std::string> &output_names)
{
    //find x10 XOR y10, note the output
    std::string x = std::format("x{:02}", i);
    std::string y = std::format("y{:02}", i);
    std::string z = std::format("z{:02}", i);
    auto itr = std::find_if(gates.begin(), gates.end(), [&x, &y](gate &a) {
        return a.in_name1 == x && a.in_name2 == y && a.op == gate_operation::gate_xor;
    });

    //find a command that uses the output with XOR op
    auto itr2 = std::find_if(gates.begin(), gates.end(), [itr](gate &a) {
        return (a.in_name1 == itr->out_name || a.in_name2 == itr->out_name) && a.op == gate_operation::gate_xor;
    });

    //find the original one we want to swap
    auto itr3 = std::find_if(gates.begin(), gates.end(), [itr, &z](gate &a) { return a.out_name == z; });

    //swap the outputs
    output_names.push_back(itr3->out_name);
    output_names.push_back(itr2->out_name);
    std::swap(itr3->out_name, itr2->out_name);
}

void repair2(gate_list_t &gates, size_t i, std::vector<std::string> &output_names)
{
    //find x10 XOR y10, note the output
    std::string x = std::format("x{:02}", i);
    std::string y = std::format("y{:02}", i);
    auto itr = std::find_if(gates.begin(), gates.end(), [&x, &y](gate &a) {
        return a.in_name1 == x && a.in_name2 == y && a.op == gate_operation::gate_xor;
    });

    //find a command that uses the output with XOR op
    auto itr2 = std::find_if(gates.begin(), gates.end(), [&x, &y](gate &a) {
        return a.in_name1 == x && a.in_name2 == y && a.op == gate_operation::gate_and;
    });

    //swap the outputs
    output_names.push_back(itr->out_name);
    output_names.push_back(itr2->out_name);
    std::swap(itr->out_name, itr2->out_name);
}

int main()
{
    auto [wire_map, gate_list] = read_file();
    size_t input_bitsize = wire_map.size() / 2U;

    int iter{ 0 };
    while (sim_tick(wire_map, gate_list, iter)) {
    };

    std::cout << get_outvalue(wire_map) << '\n';

    std::vector<std::string> output_names;
    std::unordered_map<std::string, std::string> carry_map;

    //logic for first stage is different
    {
        int i{ 1 };
        std::cout << "\nSTAGE " << i << '\n';
        std::string out = std::format("z{:02}", i);
        std::string carry = std::format("carry{:02}", i);

        auto [in1, in2, op] = print_gate(gate_list, out);
        carry_map[carry] = in2;
        std::cout << carry << " = " << in2 << '\n';

        print_gate(gate_list, in1);
        print_gate(gate_list, in2);
    }

    for (size_t i = 2; i < input_bitsize; i++) {
        std::cout << "\nSTAGE " << i << '\n';
        std::string out = std::format("z{:02}", i);
        std::string carry = std::format("carry{:02}", i);
        std::string last_carry = std::format("carry{:02}", i - 1);
        std::string carry_in1;
        std::string carry_in2;

        {
            auto [in1, in2, op] = print_gate(gate_list, out);
            //need a check here that in1 and in2 are not x, y
            //stage 10 fails this
            //need a check here that the operation is XOR
            //stage 21, 33 fails this
            if (in1.starts_with('x') || op != gate_operation::gate_xor) {
                std::cout << "Repairing...\n";
                repair1(gate_list, i, output_names);

                //restart the loop
                std::cout << "Restarting stage\n";
                i--;
                continue;
            }

            //one of these will have xn and yn as inputs
            auto [in1_1, in2_1, op1] = get_inputs(gate_list, in1);
            auto [in1_2, in2_2, op2] = get_inputs(gate_list, in2);

            if (in1_1.starts_with('x') && in2_1.starts_with('y')) {
                //need a check here that the operation is XOR
                //stage 39 fails this
                if (op1 != gate_operation::gate_xor) {
                    std::cout << "Repairing...\n";
                    repair2(gate_list, i, output_names);

                    //restart the loop
                    std::cout << "Restarting stage\n";
                    i--;
                    continue;
                }
                print_gate(gate_list, in1);
                print_gate(gate_list, in2);
                carry_map[carry] = in2;
                std::cout << carry << " = " << in2 << '\n';
                carry_in1 = in1_2;
                carry_in2 = in2_2;
            } else {
                //need a check here that the operation is XOR
                //stage 39 fails this
                if (op2 != gate_operation::gate_xor) {
                    std::cout << "Repairing...\n";
                    repair2(gate_list, i, output_names);

                    //restart the loop
                    std::cout << "Restarting stage\n";
                    i--;
                    continue;
                }
                print_gate(gate_list, in2);
                print_gate(gate_list, in1);
                carry_map[carry] = in1;
                std::cout << carry << " = " << in1 << '\n';
                carry_in1 = in1_1;
                carry_in2 = in2_1;
            }
        }

        auto [in1_1, in2_1, op1] = get_inputs(gate_list, carry_in1);
        auto [in1_2, in2_2, op2] = get_inputs(gate_list, carry_in2);

        if (in1_1.starts_with('x') && in2_1.starts_with('y')) {
            if (carry_map[last_carry] != in1_2 && carry_map[last_carry] != in2_2)
                std::cout << "Last Carry doesnt match!\n";
            print_gate(gate_list, carry_in1);
            print_gate(gate_list, carry_in2);
            if (carry_map[last_carry] == in1_2)
                print_gate(gate_list, in2_2);
            else
                print_gate(gate_list, in1_2);
        } else {
            if (carry_map[last_carry] != in1_1 && carry_map[last_carry] != in2_1)
                std::cout << "Last Carry doesnt match!\n";
            print_gate(gate_list, carry_in2);
            print_gate(gate_list, carry_in1);
            if (carry_map[last_carry] == in1_1)
                print_gate(gate_list, in2_1);
            else
                print_gate(gate_list, in1_1);
        }
    }

    std::sort(output_names.begin(), output_names.end());

    std::cout << output_names.at(0);

    for (size_t i : std::views::iota(1U, output_names.size()))
        std::cout << ',' << output_names.at(i);

    std::cout << '\n';
}