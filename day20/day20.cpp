
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <array>
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

template <size_t n_t>
using cheatmap_t = std::array<std::array<long, n_t>, n_t>;

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

template <size_t n_t>
bool find_shortcuts(const maze_map_t &map, racer_state &state, xy_pos_t dim, xy_pos_t end,
                    std::vector<shortcut> &shortcuts, const cheatmap_t<n_t> &cheatmap, long limit)
{
    static constexpr int n = static_cast<int>(n_t - 1);
    //check for any shortcuts
    for (int j = -n; j <= n; j++) {
        for (int i = -n; i <= n; i++) {
            size_t abs_i = std::abs(i);
            size_t abs_j = std::abs(j);
            if (cheatmap.at(abs_j).at(abs_i) == std::numeric_limits<long>::max())
                continue;
            xy_pos_t cut_pos = state.pos + xy_pos_t{ i, j };
            if (!is_pos_on_map(cut_pos, dim) || map.at(cut_pos.second).at(cut_pos.first).has_wall)
                continue;
            long cut_score = map.at(cut_pos.second).at(cut_pos.first).score;
            long current_score = map.at(state.pos.second).at(state.pos.first).score;
            long cheatmap_score = cheatmap.at(abs_j).at(abs_i);
            if (cut_score - current_score - cheatmap_score >= limit)
                shortcuts.push_back(shortcut{
                    .start = state.pos, .end = cut_pos, .time_saved = cut_score - current_score - cheatmap_score });
        }
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

template <size_t n_t>
constexpr cheatmap_t<n_t + 1> create_cheatmap()
{
    std::array<std::array<long, n_t + 1>, n_t + 1> cheatmap{ 0 };
    for (size_t j = 0; j < cheatmap.size(); j++) {
        for (size_t i = 0; i < cheatmap.at(j).size(); i++) {
            if (i + j > n_t)
                cheatmap.at(j).at(i) = std::numeric_limits<long>::max();
            else
                cheatmap.at(j).at(i) = i + j;
        }
    }
    return cheatmap;
}

int main()
{
    auto [map, start, end] = read_file();
    xy_pos_t dim = xy_pos_t{ map.at(0).size(), map.size() };

    racer_state state{ .pos = start };

    while (trace_track(map, state, end)) {
    }

    static constexpr auto cheatmap_part1 = create_cheatmap<2>();

    racer_state state_part1{ .pos = start };
    std::vector<shortcut> shortcuts_part1;
    while (find_shortcuts(map, state_part1, dim, end, shortcuts_part1, cheatmap_part1, 100)) {
    }

    std::cout << shortcuts_part1.size() << '\n';

    static constexpr auto cheatmap_part2 = create_cheatmap<20>();

    racer_state state_part2{ .pos = start };
    std::vector<shortcut> shortcuts_part2;
    while (find_shortcuts(map, state_part2, dim, end, shortcuts_part2, cheatmap_part2, 100)) {
    }

    std::cout << shortcuts_part2.size() << '\n';
}