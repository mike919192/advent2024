
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <array>
#include <algorithm>
#include <list>
#include <iostream>
#include <ranges>
#include "advent.h"

struct cell {
    bool has_wall{ false };
    bool is_visited{ false };
    long score = std::numeric_limits<long>::max();
};

struct endpoint {
    xy_pos_t pos{ 0, 0 };
};

using maze_row_t = std::vector<cell>;
using maze_map_t = std::vector<maze_row_t>;

struct reindeer_state {
    xy_pos_t pos{ 0, 0 };
    xy_pos_t dir{ 1, 0 };
    long score{ 0 };
};

static constexpr std::array<xy_pos_t, 4> move_dirs = { xy_pos_t{ 1, 0 }, xy_pos_t{ 0, 1 }, xy_pos_t{ -1, 0 },
                                                       xy_pos_t{ 0, -1 } };

std::tuple<maze_map_t, xy_pos_t, xy_pos_t> read_file()
{
    std::ifstream infile("input.txt");
    maze_map_t csv_rows;
    xy_pos_t start;
    xy_pos_t end;
    int y{ 0 };

    for (std::string line; std::getline(infile, line);) {
        if (line.empty())
            break;
        std::istringstream ss(line);
        maze_row_t row;
        int x{ 0 };

        char value{ 0 };
        while (ss >> value) {
            switch (value) {
            case '#':
                row.push_back(cell{ .has_wall = true });
                break;
            case '.':
                row.push_back(cell{});
                break;
            case 'S':
                start = xy_pos_t{ x, y };
                row.push_back(cell{});
                break;
            case 'E':
                end = xy_pos_t{ x, y };
                row.push_back(cell{});
                break;
            default:
                throw std::runtime_error("Error parsing logic!");
            }
            x++;
        }

        csv_rows.push_back(std::move(row));
        y++;
    }

    return { csv_rows, start, end };
}

bool visit_neighbors(maze_map_t &map, reindeer_state &state, xy_pos_t end, std::list<reindeer_state> &sources)
{
    //update score of neighbors
    for (auto dir : move_dirs) {
        xy_pos_t next_pos = state.pos + dir;
        if (map.at(next_pos.second).at(next_pos.first).has_wall ||
            map.at(next_pos.second).at(next_pos.first).is_visited)
            continue;
        long score = state.score + (dir == state.dir ? 1 : 1001);
        map.at(next_pos.second).at(next_pos.first).score =
            std::min(score, map.at(next_pos.second).at(next_pos.first).score);
        sources.push_back(reindeer_state{ .pos = next_pos, .dir = dir, .score = score });
    }

    map.at(state.pos.second).at(state.pos.first).is_visited = true;

    //choose new source
    auto new_source =
        std::min_element(sources.begin(), sources.end(), [](auto &a, auto &b) { return a.score < b.score; });

    state = *new_source;
    sources.erase(new_source);

    if (state.pos == end)
        return false;

    return true;
}

bool backtrack_neighbors(maze_map_t &map, reindeer_state &state, xy_pos_t end, std::list<reindeer_state> &sources,
                         std::vector<xy_pos_t> &path_points)
{
    //update score of neighbors
    for (auto dir : move_dirs) {
        xy_pos_t next_pos = state.pos - dir;
        if (map.at(next_pos.second).at(next_pos.first).has_wall ||
            map.at(next_pos.second).at(next_pos.first).is_visited)
            continue;
        long score1 = state.score - 1; // (dir == state.dir ? 1 : 1001);
        long score2 = state.score - 1001; //(dir == state.dir ? 1 : 1001);
        if (map.at(next_pos.second).at(next_pos.first).score == score1) {
            sources.push_back(reindeer_state{ .pos = next_pos, .dir = dir, .score = score1 });
            auto itr = std::find(path_points.begin(), path_points.end(), next_pos);
            if (itr == path_points.end()) {
                std::cout << "Counting x:" << next_pos.first << " y:" << next_pos.second << "\n";
                path_points.push_back(next_pos);
            }
        } else if (map.at(next_pos.second).at(next_pos.first).score == score2) {
            sources.push_back(reindeer_state{ .pos = next_pos, .dir = dir, .score = score1 });
            sources.push_back(reindeer_state{ .pos = next_pos, .dir = dir, .score = score2 });
            auto itr = std::find(path_points.begin(), path_points.end(), next_pos);
            if (itr == path_points.end()) {
                std::cout << "Counting x:" << next_pos.first << " y:" << next_pos.second << "\n";
                path_points.push_back(next_pos);
            }
        }
    }

    map.at(state.pos.second).at(state.pos.first).is_visited = true;

    if (sources.size() == 0)
        return false;

    //choose new source
    auto new_source =
        std::max_element(sources.begin(), sources.end(), [](auto &a, auto &b) { return a.score < b.score; });

    state = *new_source;
    sources.erase(new_source);

    //count++;

    if (state.pos == end)
        return false;

    return true;
}

void print_path(const maze_map_t &map, const std::vector<xy_pos_t> &points)
{
    std::ofstream out("output.txt");
    std::vector<std::vector<char>> print_output;
    print_output.reserve(map.size());

    for (size_t y : std::views::iota(0u, map.size())) {
        print_output.emplace_back();
        for (size_t x : std::views::iota(0u, map.at(y).size())) {
            if (map.at(y).at(x).has_wall)
                print_output.at(y).push_back('#');
            else
                print_output.at(y).push_back('.');
        }
    }

    for (const auto &point : points) {
        print_output.at(point.second).at(point.first) = 'O';
    }

    for (const auto &line : print_output) {
        for (const auto &ch : line) {
            out << ch;
        }
        out << '\n';
    }
}

int main()
{
    auto [map, start, end] = read_file();

    std::vector<reindeer_state> branches;
    reindeer_state state{ .pos = start };
    map.at(start.second).at(start.first).score = 0;
    std::list<reindeer_state> sources;

    while (visit_neighbors(map, state, end, sources)) {
    }

    std::cout << state.score << '\n';

    //reset is_visited
    for (auto &row : map)
        for (auto &cell : row)
            cell.is_visited = false;

    sources.clear();
    std::vector<xy_pos_t> path_points;
    path_points.push_back(end);
    while (backtrack_neighbors(map, state, start, sources, path_points)) {
    }

    std::cout << path_points.size() << '\n';

    print_path(map, path_points);
}