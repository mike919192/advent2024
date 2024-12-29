
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <ranges>
#include <list>
#include <algorithm>
#include <iostream>
#include "advent.hpp"

struct cell {
    bool has_wall{ false };
    bool is_visited{ false };
    long score = std::numeric_limits<long>::max();
};

using int_row_t = std::vector<long>;
using int_rows_t = std::vector<int_row_t>;
using maze_row_t = std::vector<cell>;
using maze_map_t = std::vector<maze_row_t>;

struct cell_state {
    xy_pos_t pos{ 0, 0 };
    long score{ 0 };
};

static constexpr std::array<xy_pos_t, 4> move_dirs = { xy_pos_t{ 1, 0 }, xy_pos_t{ 0, 1 }, xy_pos_t{ -1, 0 },
                                                       xy_pos_t{ 0, -1 } };

int_rows_t read_file(const char *filename)
{
    std::ifstream infile(filename);
    int_rows_t rows;

    for (std::string line; std::getline(infile, line);) {
        //if line is blank then read second section
        if (line.empty())
            break;

        std::istringstream ss(line);

        long x{ 0 };
        char comma{ 0 };
        long y{ 0 };
        ss >> x >> comma >> y;
        rows.emplace_back();
        rows.back().push_back(x);
        rows.back().push_back(y);
    }

    return rows;
}

maze_map_t build_map(const int_rows_t &bytes, xy_pos_t dim, int num)
{
    maze_map_t map;
    map.reserve(dim.second);

    for (int i : std::views::iota(0, dim.second))
        map.emplace_back(dim.first);

    int i{ 0 };
    for (const auto &byte : bytes) {
        map.at(byte.at(1)).at(byte.at(0)).has_wall = true;
        i++;
        if (i >= num)
            break;
    }

    return map;
}

bool visit_neighbors(maze_map_t &map, cell_state &state, xy_pos_t end, std::list<cell_state> &sources, xy_pos_t dim)
{
    //update score of neighbors
    for (auto dir : move_dirs) {
        xy_pos_t next_pos = state.pos + dir;
        if (!is_pos_on_map(next_pos, dim) || map.at(next_pos.second).at(next_pos.first).has_wall ||
            map.at(next_pos.second).at(next_pos.first).is_visited)
            continue;
        long score = state.score + 1;
        if (score < map.at(next_pos.second).at(next_pos.first).score)
            map.at(next_pos.second).at(next_pos.first).score = score;

        auto itr = std::find_if(sources.begin(), sources.end(), [next_pos](auto &a) { return a.pos == next_pos; });
        if (itr == sources.end())
            sources.push_back(cell_state{ .pos = next_pos, .score = score });
        else if (score < (*itr).score)
            (*itr).score = score;
    }

    map.at(state.pos.second).at(state.pos.first).is_visited = true;

    //choose new source
    auto new_source =
        std::min_element(sources.begin(), sources.end(), [](auto &a, auto &b) { return a.score < b.score; });

    state = *new_source;
    sources.erase(new_source);

    if (state.pos == end) {
        map.at(state.pos.second).at(state.pos.first).score = state.score;
        return false;
    }

    return true;
}

int main()
{
    // auto bytes = read_file("test.txt");
    // xy_pos_t end{ 6, 6 };
    // int num_bytes{ 12 };

    auto bytes = read_file("input.txt");
    xy_pos_t end{ 70, 70 };
    int num_bytes{ 1024 };

    xy_pos_t start{ 0, 0 };
    xy_pos_t dim = end + xy_pos_t{ 1, 1 };

    auto map = build_map(bytes, dim, num_bytes);

    cell_state state{ .pos = start };
    map.at(start.second).at(start.first).score = 0;
    std::list<cell_state> sources;

    while (visit_neighbors(map, state, end, sources, dim)) {
    }

    std::cout << state.score << '\n';
}