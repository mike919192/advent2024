
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <future>

bool is_pos_on_map(int pos_x, int pos_y, int dim_x, int dim_y)
{
    return !(pos_x < 0 || pos_y < 0 || pos_x >= dim_x || pos_y >= dim_y);
}

struct patrol_guard {
    std::pair<int, int> pos{ 0, 0 };
    std::pair<int, int> dir{ 0, -1 };

    bool operator==(const patrol_guard &) const = default;

    std::pair<int, int> peek_next_position()
    {
        return std::pair<int, int>{ pos.first + dir.first, pos.second + dir.second };
    }

    void move_next_position()
    {
        pos.first += dir.first;
        pos.second += dir.second;
    }

    void rotate()
    {
        if (dir == std::pair<int, int>{ 0, -1 })
            dir = std::pair<int, int>{ 1, 0 };
        else if (dir == std::pair<int, int>{ 1, 0 })
            dir = std::pair<int, int>{ 0, 1 };
        else if (dir == std::pair<int, int>{ 0, 1 })
            dir = std::pair<int, int>{ -1, 0 };
        else if (dir == std::pair<int, int>{ -1, 0 })
            dir = std::pair<int, int>{ 0, -1 };
        else
            throw std::runtime_error("Error in rotate logic!");
    }

    bool is_on_map(int dim_x, int dim_y)
    {
        return is_pos_on_map(pos.first, pos.second, dim_x, dim_y);
    }
};

struct cell {
    bool has_obstacle{ false };
    bool is_visited{ false };
    patrol_guard visited_guard;
};

cell char_to_cell(char c)
{
    if (c == '#')
        return cell{ .has_obstacle = true };
    else
        return cell{};
}

std::vector<std::vector<char>> read_file()
{
    std::ifstream infile("input.txt");
    std::vector<std::vector<char>> csv_rows;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        std::vector<char> row;

        char value{ 0 };
        while (ss >> value) {
            row.push_back(value);
        }

        csv_rows.push_back(std::move(row));
    }

    return csv_rows;
}

std::tuple<std::vector<std::vector<cell>>, patrol_guard>
construct_game_state(const std::vector<std::vector<char>> &map_chars)
{
    std::vector<std::vector<cell>> map(map_chars.size());
    patrol_guard guard;

    for (size_t i = 0; i < map_chars.size(); i++) {
        map.at(i).resize(map_chars.at(i).size());
        std::transform(map_chars.at(i).begin(), map_chars.at(i).end(), map.at(i).begin(), char_to_cell);
    }

    //find the guard and set his position
    for (size_t i = 0; i < map_chars.size(); i++) {
        auto itr1 = std::find(map_chars.at(i).begin(), map_chars.at(i).end(), '^');
        if (itr1 != map_chars.at(i).end()) {
            auto index1 = std::distance(map_chars.at(i).begin(), itr1);
            guard.pos.first = index1;
            guard.pos.second = i;
            break;
        }
    }

    return { map, guard };
}

bool execute_game_tick(std::vector<std::vector<cell>> &map, patrol_guard &guard, const patrol_guard &original_guard)
{
    if (map.at(guard.pos.second).at(guard.pos.first).is_visited == false) {
        map.at(guard.pos.second).at(guard.pos.first).is_visited = true;
        map.at(guard.pos.second).at(guard.pos.first).visited_guard = guard;
    }

    auto next_pos = guard.peek_next_position();

    //is the next position an obstacle?
    if (is_pos_on_map(next_pos.first, next_pos.second, map.at(0).size(), map.size()) &&
        map.at(next_pos.second).at(next_pos.first).has_obstacle) {
        guard.rotate();
    } else {
        guard.move_next_position();
    }

    return guard.is_on_map(map.at(0).size(), map.size()) &&
           map.at(guard.pos.second).at(guard.pos.first).visited_guard != guard;
}

bool part2_thread(const std::vector<std::vector<cell>> &original_map, const patrol_guard &original_guard, size_t i,
                  size_t j)
{
    auto copy_map = original_map;
    auto copy_guard = original_guard;

    //set the obstacle
    copy_map.at(j).at(i).has_obstacle = true;

    while (execute_game_tick(copy_map, copy_guard, original_guard))
        ;

    if (copy_guard.is_on_map(copy_map.at(0).size(), copy_map.size()) &&
        copy_map.at(copy_guard.pos.second).at(copy_guard.pos.first).visited_guard == copy_guard)
        return true;
    return false;
}

int main()
{
    //part 1
    auto map_chars = read_file();

    auto [map, guard] = construct_game_state(map_chars);

    const auto original_map = map;
    const patrol_guard original_guard = guard;

    while (execute_game_tick(map, guard, original_guard))
        ;

    int visited_sum{ 0 };

    auto increment_sum = [&visited_sum](const cell &n) {
        if (n.is_visited)
            visited_sum++;
    };

    //sum up the is_visited
    for (const auto &map_row : map) {
        std::for_each(map_row.begin(), map_row.end(), increment_sum);
    }

    std::cout << visited_sum << '\n';

    //part 2
    int is_looped_sum{ 0 };
    for (size_t j = 0; j < original_map.size(); j++) {
        for (size_t i = 0; i < original_map.at(0).size(); i += 5) {
            std::future<bool> ret1 = std::async(&part2_thread, original_map, original_guard, i, j);
            std::future<bool> ret2 = std::async(&part2_thread, original_map, original_guard, i + 1, j);
            std::future<bool> ret3 = std::async(&part2_thread, original_map, original_guard, i + 2, j);
            std::future<bool> ret4 = std::async(&part2_thread, original_map, original_guard, i + 3, j);
            std::future<bool> ret5 = std::async(&part2_thread, original_map, original_guard, i + 4, j);

            if (ret1.get())
                is_looped_sum++;
            if (ret2.get())
                is_looped_sum++;
            if (ret3.get())
                is_looped_sum++;
            if (ret4.get())
                is_looped_sum++;
            if (ret5.get())
                is_looped_sum++;
        }
    }
    std::cout << is_looped_sum << '\n';
}