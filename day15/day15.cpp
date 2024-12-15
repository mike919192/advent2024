
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <ranges>
#include <iostream>

template <typename T, typename U, typename V, typename W>
auto operator+(const std::pair<T, U> &l, const std::pair<V, W> &r)
{
    return std::pair<T, U>{ l.first + r.first, l.second + r.second };
}

template <typename T, typename U, typename V, typename W>
auto operator-(const std::pair<T, U> &l, const std::pair<V, W> &r)
{
    return std::pair<T, U>{ l.first - r.first, l.second - r.second };
}

template <typename T, typename U>
auto operator-(const std::pair<T, U> &l)
{
    return std::pair<T, U>{ -l.first, -l.second };
}

using xy_pos_t = std::pair<int, int>;

struct cell {
    bool has_wall{ false };
    bool has_box{ false };

    bool is_empty()
    {
        return !has_wall && !has_box;
    }
};

struct robot {
    xy_pos_t pos{ 0, 0 };
};

using warehouse_row_t = std::vector<cell>;
using warehouse_map_t = std::vector<warehouse_row_t>;
using move_list_t = std::vector<xy_pos_t>;

std::tuple<warehouse_map_t, robot, move_list_t> read_file()
{
    std::ifstream infile("input.txt");
    warehouse_map_t csv_rows;
    robot rob;
    move_list_t moves;
    int y{ 0 };

    for (std::string line; std::getline(infile, line);) {
        if (line.empty())
            break;
        std::istringstream ss(line);
        warehouse_row_t row;
        int x{ 0 };

        char value{ 0 };
        while (ss >> value) {
            switch (value) {
            case '#':
                row.push_back(cell{ .has_wall = true });
                break;
            case 'O':
                row.push_back(cell{ .has_box = true });
                break;
            case '.':
                row.push_back(cell{});
                break;
            case '@':
                row.push_back(cell{});
                rob.pos = xy_pos_t{ x, y };
                break;
            default:
                throw std::runtime_error("Error parsing logic!");
            }
            x++;
        }

        csv_rows.push_back(std::move(row));
        y++;
    }

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        char value{ 0 };
        while (ss >> value) {
            switch (value) {
            case '>':
                moves.push_back(xy_pos_t{ 1, 0 });
                break;
            case 'v':
                moves.push_back(xy_pos_t{ 0, 1 });
                break;
            case '<':
                moves.push_back(xy_pos_t{ -1, 0 });
                break;
            case '^':
                moves.push_back(xy_pos_t{ 0, -1 });
                break;
            default:
                throw std::runtime_error("Error parsing logic!");
            }
        }
    }

    return { csv_rows, rob, moves };
}

void execute_move(warehouse_map_t &map, robot &rob, xy_pos_t move)
{
    //look in the direction of the move until we find either open space or wall
    xy_pos_t look_ahead = rob.pos + move;
    cell look_ahead_cell = map.at(look_ahead.second).at(look_ahead.first);
    while (!look_ahead_cell.has_wall && !look_ahead_cell.is_empty()) {
        look_ahead = look_ahead + move;
        look_ahead_cell = map.at(look_ahead.second).at(look_ahead.first);
    }
    //if we find a wall we can't execute the move
    if (look_ahead_cell.has_wall)
        return;
    //if we find an open space then we can push all boxes
    while (look_ahead != rob.pos + move) {
        map.at(look_ahead.second).at(look_ahead.first).has_box = true;
        look_ahead = look_ahead - move;
    }
    map.at(look_ahead.second).at(look_ahead.first).has_box = false;
    rob.pos = look_ahead;
}

void print_map(warehouse_map_t &map, robot &rob, int iteration, xy_pos_t move)
{
    std::ofstream out("output.txt", std::ios_base::app);
    out << iteration << " " << move.first << " " << move.second << '\n';
    std::vector<std::string> print_output;
    print_output.reserve(map.size());

    for (const auto &row : map) {
        print_output.emplace_back();
        for (const auto &cell : row) {
            if (cell.has_wall)
                print_output.back().push_back('#');
            else if (cell.has_box)
                print_output.back().push_back('O');
            else
                print_output.back().push_back('.');
        }
    }

    print_output.at(rob.pos.second).at(rob.pos.first) = '@';

    for (const auto &line : print_output) {
        out << line << '\n';
    }
}

long score_map(const warehouse_map_t & map)
{
    long score {0};
    for (size_t y : std::views::iota(0u, map.size())) {
        for (size_t x : std::views::iota(0u, map.at(y).size())) {
            if (map.at(y).at(x).has_box)
                score += y * 100 + x;
        }
    }

    return score;
}

int main()
{
    auto [map, rob, moves] = read_file();

    int iteration{ 0 };
    //print_map(map, rob, iteration, xy_pos_t{ 0, 0 });
    for (const auto &move : moves) {
        iteration++;
        execute_move(map, rob, move);
        //print_map(map, rob, iteration, move);
    }

    long score = score_map(map);
    std::cout << score << '\n';
}