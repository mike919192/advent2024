
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <array>
#include <list>
#include <numeric>
#include <iostream>
#include "advent.hpp"

struct cell {
    bool has_wall{ false };
    bool is_visited{ false };
    long score = std::numeric_limits<long>::max();
    xy_pos_t dir{ 0, 0 };
};

using maze_row_t = std::vector<cell>;
using maze_map_t = std::vector<maze_row_t>;

struct racer_state {
    xy_pos_t pos{ 0, 0 };
    xy_pos_t dir{ 1, 0 };
    long score{ 0 };
};

struct shortcut {
    xy_pos_t start{ 0, 0 };
    xy_pos_t end{ 0, 0 };
    long time_saved{ 0 };
};

static constexpr std::array<xy_pos_t, 4> move_dirs = { xy_pos_t{ 1, 0 }, xy_pos_t{ 0, 1 }, xy_pos_t{ -1, 0 },
                                                       xy_pos_t{ 0, -1 } };

static constexpr std::array<xy_pos_t, 4> shortcut_dirs = { xy_pos_t{ 2, 0 }, xy_pos_t{ 0, 2 }, xy_pos_t{ -2, 0 },
                                                           xy_pos_t{ 0, -2 } };

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

bool trace_track(maze_map_t &map, racer_state &state, xy_pos_t end)
{
    //update score of neighbors
    for (auto dir : move_dirs) {
        xy_pos_t next_pos = state.pos + dir;
        if (map.at(next_pos.second).at(next_pos.first).has_wall ||
            map.at(next_pos.second).at(next_pos.first).is_visited)
            continue;
        long score = state.score + 1;
        map.at(state.pos.second).at(state.pos.first).is_visited = true;
        map.at(state.pos.second).at(state.pos.first).score = state.score;
        map.at(state.pos.second).at(state.pos.first).dir = dir;
        state = racer_state{ .pos = next_pos, .dir = dir, .score = score };
        break;
    }

    if (state.pos == end) {
        map.at(state.pos.second).at(state.pos.first).score = state.score;
        map.at(state.pos.second).at(state.pos.first).dir = state.dir;
        return false;
    }

    return true;
}

bool find_shortcuts(maze_map_t &map, racer_state &state, xy_pos_t dim, xy_pos_t end, std::vector<shortcut> &shortcuts)
{
    //check for any shortcuts
    for (auto cut_dir : shortcut_dirs) {
        xy_pos_t cut_pos = state.pos + cut_dir;
        if (!is_pos_on_map(cut_pos, dim) || map.at(cut_pos.second).at(cut_pos.first).has_wall)
            continue;
        long cut_score = map.at(cut_pos.second).at(cut_pos.first).score;
        long current_score = map.at(state.pos.second).at(state.pos.first).score;
        if (cut_score - current_score > 2)
            shortcuts.push_back(
                shortcut{ .start = state.pos, .end = cut_pos, .time_saved = cut_score - current_score - 2 });
    }

    //get the direction and update the state
    xy_pos_t dir = map.at(state.pos.second).at(state.pos.first).dir;

    state.pos = state.pos + dir;
    state.score++;
    state.dir = dir;

    if (state.pos == end) {
        return false;
    }

    return true;
}

int main()
{
    auto [map, start, end] = read_file();
    xy_pos_t dim = xy_pos_t{ map.at(0).size(), map.size() };

    racer_state state{ .pos = start };

    while (trace_track(map, state, end)) {
    }

    racer_state state2{ .pos = start };
    std::vector<shortcut> shortcuts;
    while (find_shortcuts(map, state2, dim, end, shortcuts)) {
    }

    auto count_shortcuts = [](auto init, auto &a) { return a.time_saved >= 100 ? init + 1 : init; };

    int result = std::accumulate(shortcuts.begin(), shortcuts.end(), 0, count_shortcuts);
    std::cout << result << '\n';
}