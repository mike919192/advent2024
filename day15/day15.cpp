
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <ranges>
#include <iostream>
#include <algorithm>

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
};

struct box {
    xy_pos_t pos1{ 0, 0 };
    xy_pos_t pos2{ 0, 0 };

    box(xy_pos_t pos) : pos1(pos), pos2(pos)
    {
    }
};

struct robot {
    xy_pos_t pos{ 0, 0 };
};

using warehouse_row_t = std::vector<cell>;
using warehouse_map_t = std::vector<warehouse_row_t>;
using move_list_t = std::vector<xy_pos_t>;
using box_list_t = std::vector<box>;

std::tuple<warehouse_map_t, box_list_t, robot, move_list_t> read_file()
{
    std::ifstream infile("input.txt");
    warehouse_map_t csv_rows;
    robot rob;
    move_list_t moves;
    box_list_t boxes;
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
                row.push_back(cell{});
                boxes.push_back(box(xy_pos_t{ x, y }));
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

    return { csv_rows, boxes, rob, moves };
}

bool peek_box_push(warehouse_map_t &map, box_list_t &boxes, box &b, xy_pos_t move)
{
    xy_pos_t pos1_push = b.pos1 + move;
    if (map.at(pos1_push.second).at(pos1_push.first).has_wall)
        return false;
    //see if there is any boxes we push into
    auto itr = std::find_if(boxes.begin(), boxes.end(), [pos1_push](auto &a) { return a.pos1 == pos1_push; });
    if (itr == boxes.end())
        return true;
    else
        return peek_box_push(map, boxes, (*itr), move);
}

void do_box_push(warehouse_map_t &map, box_list_t &boxes, box &b, xy_pos_t move)
{
    xy_pos_t pos1_push = b.pos1 + move;
    
    //see if there is any boxes we push into
    auto itr = std::find_if(boxes.begin(), boxes.end(), [pos1_push](auto &a) { return a.pos1 == pos1_push; });
    if (itr != boxes.end())
        do_box_push(map, boxes, (*itr), move);

    b.pos1 = b.pos1 + move;
    b.pos2 = b.pos2 + move;
}

void execute_move(warehouse_map_t &map, box_list_t &boxes, robot &rob, xy_pos_t move)
{
    //look in the direction of the move until we find either open space or wall
    xy_pos_t look_ahead = rob.pos + move;
    cell look_ahead_cell = map.at(look_ahead.second).at(look_ahead.first);

    if (look_ahead_cell.has_wall)
        return;

    auto itr = std::find_if(boxes.begin(), boxes.end(), [look_ahead](auto &a) { return a.pos1 == look_ahead; });

    //no boxes so just move into empty space
    if (itr == boxes.end()) {
        rob.pos = look_ahead;
        return;
    }

    //check if boxes can be pushed
    if (peek_box_push(map, boxes, (*itr), move)) {
        do_box_push(map, boxes, (*itr), move);
        rob.pos = look_ahead;
    }
}

void print_map(const warehouse_map_t &map, const box_list_t &boxes, const robot &rob, int iteration, xy_pos_t move)
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
            else
                print_output.back().push_back('.');
        }
    }

    for (const auto &box : boxes) {
        if (box.pos1 == box.pos2) {
            print_output.at(box.pos1.second).at(box.pos1.first) = 'O';
        } else {
            print_output.at(box.pos1.second).at(box.pos1.first) = '[';
            print_output.at(box.pos2.second).at(box.pos2.first) = ']';
        }
    }

    print_output.at(rob.pos.second).at(rob.pos.first) = '@';

    for (const auto &line : print_output) {
        out << line << '\n';
    }
}

long score_map(const box_list_t & boxes)
{
    long score{ 0 };
    for (const auto & box : boxes)
        score += box.pos1.second * 100 + box.pos1.first;

    return score;
}

int main()
{
    auto [map, boxes, rob, moves] = read_file();

    int iteration{ 0 };
    //print_map(map, boxes, rob, iteration, xy_pos_t{ 0, 0 });
    for (const auto &move : moves) {
        iteration++;
        execute_move(map, boxes, rob, move);
        //print_map(map, boxes, rob, iteration, move);
    }

    long score = score_map(boxes);
    std::cout << score << '\n';
}