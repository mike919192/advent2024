
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <iostream>

using conn_row_t = std::array<std::string, 2>;
using conn_list_t = std::vector<conn_row_t>;
using conn_map_t = std::unordered_map<std::string, std::vector<std::string>>;
using interconn_row_t = std::vector<std::string>;
using interconn_list_t = std::vector<interconn_row_t>;

conn_list_t read_file()
{
    std::ifstream infile("input.txt");
    conn_list_t codes_list;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        conn_row_t row;

        getline(ss, row.at(0), '-');
        getline(ss, row.at(1));

        codes_list.push_back(row);
    }

    return codes_list;
}

conn_map_t build_map(const conn_list_t &connects)
{
    conn_map_t conn_map;

    for (const auto &conn : connects) {
        if (!conn_map.contains(conn.at(0))) {
            conn_map[conn.at(0)].emplace_back(conn.at(1));
        } else {
            conn_map[conn.at(0)].push_back(conn.at(1));
        }

        if (!conn_map.contains(conn.at(1))) {
            conn_map[conn.at(1)].emplace_back(conn.at(0));
        } else {
            conn_map[conn.at(1)].push_back(conn.at(0));
        }
    }

    return conn_map;
}

interconn_list_t get_interconnects(const conn_map_t &conn_map)
{
    interconn_list_t interconns;

    for (const auto &conn : conn_map) {
        std::string comp1 = conn.first;
        for (size_t i : std::views::iota(1u, conn.second.size())) {
            std::string comp2 = conn.second.at(i - 1u);
            for (size_t j : std::views::iota(i, conn.second.size())) {
                std::string comp3 = conn.second.at(j);

                //check if the 3 computers are interconnected
                const interconn_row_t &row = conn_map.at(comp2);
                if (std::find(row.begin(), row.end(), comp3) != row.end()) {
                    interconn_row_t row;
                    row.push_back(comp1);
                    row.push_back(comp2);
                    row.push_back(comp3);
                    std::sort(row.begin(), row.end());
                    //check if the row is already added
                    if (std::find(interconns.begin(), interconns.end(), row) == interconns.end()) {
                        interconns.push_back(std::move(row));
                    }
                }
            }
        }
    }

    return interconns;
}

interconn_list_t get_interconns_with_t(const interconn_list_t &interconns)
{
    interconn_list_t interconns_with_t;

    for (const auto &interconn : interconns) {
        if (std::any_of(interconn.begin(), interconn.end(), [](auto &i) { return i.starts_with('t'); })) {
            interconns_with_t.push_back(interconn);
        }
    }

    return interconns_with_t;
}

interconn_list_t get_max_interconnects(const conn_map_t &conn_map)
{
    interconn_list_t interconns;

    for (const auto &conn : conn_map) {
        std::string comp1 = conn.first;
        
        
        for (size_t i : std::views::iota(1u, conn.second.size())) {
            std::string comp2 = conn.second.at(i - 1u);
            interconn_row_t temp_row;
            temp_row.push_back(comp1);
            temp_row.push_back(comp2);
            for (size_t j : std::views::iota(i, conn.second.size())) {
                std::string comp3 = conn.second.at(j);

                bool all_in {true};
                for (const auto & row_el : temp_row) {
                    const interconn_row_t &row = conn_map.at(row_el);
                    if (std::find(row.begin(), row.end(), comp3) != row.end()) {
                        all_in &= true;
                    } else {
                        all_in = false;
                        break;
                    }
                }

                if (all_in)
                    temp_row.push_back(comp3);
            }
            std::sort(temp_row.begin(), temp_row.end());

            if (std::find(interconns.begin(), interconns.end(), temp_row) == interconns.end()) {
                interconns.push_back(std::move(temp_row));
            }
        }
    }

    return interconns;
}

int main()
{
    auto connects = read_file();

    auto conn_map = build_map(connects);

    auto interconns = get_interconnects(conn_map);

    auto interconns_with_t = get_interconns_with_t(interconns);

    std::cout << interconns_with_t.size() << '\n';

    auto max_interconns = get_max_interconnects(conn_map);

    auto max_el = std::max_element(max_interconns.begin(), max_interconns.end(), [] (auto & a, auto & b)
    {
        return a.size() < b.size();
    });

    std::cout << max_el->at(0);

    for (size_t i : std::views::iota(1u, max_el->size())) {
        std::cout << ',' << max_el->at(i);
    }
    
    std::cout << '\n';
}