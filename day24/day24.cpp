
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>
#include <format>
#include <iostream>
#include <array>

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

using wire_map_t = std::map<std::string, bool>;
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

bool sim_tick(wire_map_t &wires, gate_list_t &gates, int& iter)
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

int main()
{
    auto [wire_map, gate_list] = read_file();
    size_t input_bitsize = wire_map.size() / 2U;

    int iter {0};
    while (sim_tick(wire_map, gate_list, iter)) {
    };

    std::cout << get_outvalue(wire_map) << '\n';

    std::string test = "z01";
    auto itr = std::find_if(gate_list.begin(), gate_list.end(), [&test](gate & a)
    {
        return a.out_name == test;
    });

    auto gate_list2 = gate_list;

    int swap_count {0};

restart_check:

    for (size_t y = 0; y <= input_bitsize; y++) {
        int64_t y_bits = y == 0 ? 0 : 1 << (y - 1U);
        for (size_t x = 0; x <= input_bitsize; x++) {
            int64_t x_bits = x == 0 ? 0 : 1 << (x - 1U);
            auto wire_map2 = create_wiremap(input_bitsize, x_bits, y_bits);

            int iter {0};
            while (sim_tick(wire_map2, gate_list2, iter)) {
            };
            
            if (iter > 100)
                std::cout << "Max iter reached\n";

            int64_t out_value = get_outvalue(wire_map2);
            if ((x_bits + y_bits) != out_value) {
                std::cout << "Should be: " << x_bits << " + " << y_bits << " = " << (x_bits + y_bits) << '\n';
                std::cout << "Is: " << out_value << '\n';

                //do swap
                std::array<size_t, 2> swap1 = {93, 98};
                std::array<size_t, 2> swap2 = {107, 261};
                size_t offset = 92;
                std::swap(gate_list2.at(swap1.at(swap_count) - offset).out_name, gate_list2.at(swap2.at(swap_count) - offset).out_name);
                swap_count++;

                //restart check
                goto restart_check;
            }
        }
    }
}