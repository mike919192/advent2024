
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ranges>
#include <numeric>
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

struct robot {
    xy_pos_t pos{ 0, 0 };
    xy_pos_t vel{ 0, 0 };
};

using robot_list_t = std::vector<robot>;

robot_list_t read_file()
{
    std::ifstream infile("input.txt");
    robot_list_t csv_rows;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);

        char label{ 0 };
        int value1{ 0 };
        int value2{ 0 };
        int value3{ 0 };
        int value4{ 0 };
        ss >> label >> label >> value1 >> label >> value2 >> label >> label >> value3 >> label >> value4;

        csv_rows.push_back(robot{ .pos = xy_pos_t{ value1, value2 }, .vel = { value3, value4 } });
    }

    return csv_rows;
}

xy_pos_t wrap_pos(xy_pos_t pos, xy_pos_t dim)
{
    while (pos.first < 0)
        pos.first += dim.first;
    while (pos.first >= dim.first)
        pos.first -= dim.first;

    while (pos.second < 0)
        pos.second += dim.second;
    while (pos.second >= dim.second)
        pos.second -= dim.second;

    return pos;
}

void simulation_tick(robot_list_t &robots, xy_pos_t dim)
{
    for (auto &robot : robots) {
        robot.pos = robot.pos + robot.vel;
        robot.pos = wrap_pos(robot.pos, dim);
    }
}

void print_robots(std::ostream &out, robot_list_t &robots, xy_pos_t dim, int seconds)
{
    out << seconds << '\n';
    std::vector<std::string> print_output;
    print_output.reserve(dim.second);

    for (int y = 0; y < dim.second; y++) {
        print_output.emplace_back(dim.first, '.');
    }

    for (const auto &robot : robots) {
        if (print_output.at(robot.pos.second).at(robot.pos.first) == '.') {
            print_output.at(robot.pos.second).at(robot.pos.first) = '1';
        } else {
            print_output.at(robot.pos.second).at(robot.pos.first)++;
        }
    }

    for (const auto &line : print_output) {
        out << line << "\n";
    }
}

void part1(robot_list_t robots, xy_pos_t dim)
{
    for (int i : std::views::iota(0, 100)) {
        simulation_tick(robots, dim);
    }

    auto quad1_count = [dim](auto init, auto &a) {
        return ((a.pos.first < dim.first / 2) && (a.pos.second < dim.second / 2)) ? init + 1 : init;
    };

    auto quad2_count = [dim](auto init, auto &a) {
        return ((a.pos.first > dim.first / 2) && (a.pos.second < dim.second / 2)) ? init + 1 : init;
    };

    auto quad3_count = [dim](auto init, auto &a) {
        return ((a.pos.first < dim.first / 2) && (a.pos.second > dim.second / 2)) ? init + 1 : init;
    };

    auto quad4_count = [dim](auto init, auto &a) {
        return ((a.pos.first > dim.first / 2) && (a.pos.second > dim.second / 2)) ? init + 1 : init;
    };

    int quad1 = std::accumulate(robots.begin(), robots.end(), 0, quad1_count);
    int quad2 = std::accumulate(robots.begin(), robots.end(), 0, quad2_count);
    int quad3 = std::accumulate(robots.begin(), robots.end(), 0, quad3_count);
    int quad4 = std::accumulate(robots.begin(), robots.end(), 0, quad4_count);

    std::cout << quad1 * quad2 * quad3 * quad4 << '\n';
}

int main()
{
    auto robots = read_file();

    //xy_pos_t dim_test {11, 7};
    xy_pos_t dim_actual{ 101, 103 };

    part1(robots, dim_actual);

    std::ofstream out("output.txt");

    for (int i : std::views::iota(0, 10000)) {
        print_robots(out, robots, dim_actual, i);
        simulation_tick(robots, dim_actual);
    }
}
