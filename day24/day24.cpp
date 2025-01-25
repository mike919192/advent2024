
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <format>
#include <iostream>

enum class gate_operation { gate_and, gate_or, gate_xor };

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
        int value;

        getline(ss, name, ':');
        ss >> value;

        wire_map[name] = value != 0;
    }

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        std::string in_name1;
        std::string op_str;
        std::string in_name2;
        std::string arrow;
        std::string out_name;
        gate_operation op{ gate_operation::gate_and };

        ss >> in_name1 >> op_str >> in_name2 >> arrow >> out_name;

        if (op_str == "AND")
            op = gate_operation::gate_and;
        else if (op_str == "OR")
            op = gate_operation::gate_or;
        else if (op_str == "XOR")
            op = gate_operation::gate_xor;
        else
            throw std::runtime_error("Error parsing gate op!");

        gate_list.push_back(gate{ .in_name1 = in_name1, .in_name2 = in_name2, .op = op, .out_name = out_name });
    }

    return { wire_map, gate_list };
}

bool sim_tick(wire_map_t &wires, gate_list_t &gates)
{
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

int main()
{
    auto [wire_map, gate_list] = read_file();

    while (sim_tick(wire_map, gate_list)) {
    };

    uint i{ 0 };
    std::string z_out_name = std::format("z{:02}", i);
    int64_t out_value{ 0 };

    while (wire_map.contains(z_out_name)) {
        int64_t value = wire_map[z_out_name];
        out_value |= value << i;
        i++;
        z_out_name = std::format("z{:02}", i);
    }

    std::cout << out_value << '\n';
}