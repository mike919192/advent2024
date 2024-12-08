
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>

template <typename T, typename U, typename V, typename W>
auto operator+(const std::pair<T, U> &l,
               const std::pair<V, W> &r) -> std::pair<decltype(l.first + r.first), decltype(l.second + r.second)>
{
    return { l.first + r.first, l.second + r.second };
}

template <typename T, typename U, typename V, typename W>
auto operator-(const std::pair<T, U> &l,
               const std::pair<V, W> &r) -> std::pair<decltype(l.first + r.first), decltype(l.second + r.second)>
{
    return { l.first - r.first, l.second - r.second };
}

using xy_pos_t = std::pair<int, int>;

struct antenna {
    xy_pos_t pos{ 0, 0 };
    char frequency{ 0 };
};

using char_row_t = std::vector<char>;
using char_map_t = std::vector<char_row_t>;
using antenna_sublist_t = std::vector<antenna>;
using antenna_list_t = std::vector<antenna_sublist_t>;

bool is_pos_on_map(xy_pos_t pos, xy_pos_t dim)
{
    return !(pos.first < 0 || pos.second < 0 || pos.first >= dim.first || pos.second >= dim.second);
}

char_map_t read_file()
{
    std::ifstream infile("input.txt");
    char_map_t csv_rows;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        char_row_t row;

        char value{ 0 };
        while (ss >> value) {
            row.push_back(value);
        }

        csv_rows.push_back(std::move(row));
    }

    return csv_rows;
}

antenna_list_t construct_antenna_list(const char_map_t &map_chars)
{
    antenna_list_t antennas;
    auto char_detect = [](char i) { return i == '.'; };

    //find the antennas and set there properties
    for (size_t i = 0; i < map_chars.size(); i++) {
        size_t offset{ 0 };
        char_row_t::const_iterator itr1;
        while ((itr1 = std::find_if_not(std::next(map_chars.at(i).begin(), offset), map_chars.at(i).end(),
                                        char_detect)) != map_chars.at(i).end()) {
            auto index1 = std::distance(map_chars.at(i).begin(), itr1);
            offset = index1 + 1;
            char freq = map_chars.at(i).at(index1);

            //find if their is already an antenna of this frequency
            auto itr2 = std::find_if(antennas.begin(), antennas.end(),
                                     [freq](antenna_sublist_t i) { return i.size() > 0 && i.at(0).frequency == freq; });

            if (itr2 == antennas.end()) {
                antennas.push_back(antenna_sublist_t{});
                antennas.back().push_back(antenna{ .pos = xy_pos_t{ i, index1 }, .frequency = freq });
            } else {
                (*itr2).push_back(antenna{ .pos = xy_pos_t{ i, index1 }, .frequency = freq });
            }
        }
    }

    return antennas;
}

int calculate_antinodes(const antenna_list_t &antennas, xy_pos_t dim)
{
    int antinode_count{ 0 };
    std::vector<xy_pos_t> antinode_positions;
    for (const auto &sublist : antennas) {
        for (size_t i1{ 0 }; i1 + 1 < sublist.size(); i1++) {
            for (size_t i2 = i1 + 1; i2 < sublist.size(); i2++) {
                //use antennas at starting index and i
                xy_pos_t dist = sublist.at(i1).pos - sublist.at(i2).pos;
                xy_pos_t pos1 = sublist.at(i1).pos + dist;
                xy_pos_t pos2 = sublist.at(i2).pos - dist;

                auto itr1 = std::find(antinode_positions.begin(), antinode_positions.end(), pos1);

                if (is_pos_on_map(pos1, dim) && itr1 == antinode_positions.end()) {
                    antinode_positions.push_back(pos1);
                    antinode_count++;
                }

                auto itr2 = std::find(antinode_positions.begin(), antinode_positions.end(), pos2);

                if (is_pos_on_map(pos2, dim) && itr2 == antinode_positions.end()) {
                    antinode_positions.push_back(pos2);
                    antinode_count++;
                }
            }
        }
    }
    return antinode_count;
}

int main()
{
    const auto map_chars = read_file();

    const auto antennas = construct_antenna_list(map_chars);

    const auto antinode_count = calculate_antinodes(antennas, xy_pos_t{ map_chars.at(0).size(), map_chars.size() });

    std::cout << antinode_count << '\n';
}