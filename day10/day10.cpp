
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>
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

template <typename T>
bool vector_contains(const std::vector<T> &vec, const T &val)
{
    auto itr = std::find(vec.begin(), vec.end(), val);
    return !(itr == vec.end());
}

using int_row_t = std::vector<int>;
using int_map_t = std::vector<int_row_t>;
using xy_pos_t = std::pair<int, int>;

struct trailhead {
    xy_pos_t pos{ 0, 0 };
    int height{ 0 };
    bool operator==(const trailhead &) const = default;
};

using trailhead_list_t = std::vector<trailhead>;

bool is_pos_on_map(xy_pos_t pos, xy_pos_t dim)
{
    return !(pos.first < 0 || pos.second < 0 || pos.first >= dim.first || pos.second >= dim.second);
}

static constexpr std::array<xy_pos_t, 4> trail_dirs = { xy_pos_t{ 1, 0 }, xy_pos_t{ 0, 1 }, xy_pos_t{ -1, 0 },
                                                        xy_pos_t{ 0, -1 } };

int_map_t read_file()
{
    std::ifstream infile("input.txt");
    int_map_t csv_rows;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        int_row_t row;

        char value{ 0 };
        while (ss >> value) {
            row.push_back(value - '0');
        }

        csv_rows.push_back(std::move(row));
    }

    return csv_rows;
}

trailhead_list_t scan_for_trailheads(const int_map_t &map)
{
    trailhead_list_t trailheads;
    auto trailhead_detect = [](auto i) { return i == 0; };
    int pos_y{ 0 };
    for (const auto &map_row : map) {
        size_t offset{ 0 };
        int_row_t::const_iterator itr1;
        while ((itr1 = std::find_if(std::next(map_row.begin(), offset), map_row.end(), trailhead_detect)) !=
               map_row.end()) {
            auto pos_x = std::distance(map_row.begin(), itr1);
            offset = pos_x + 1;
            trailheads.push_back(trailhead{ .pos = xy_pos_t{ pos_x, pos_y } });
        }
        pos_y++;
    }
    return trailheads;
}

template <bool part_2_t>
void trace_trailhead(const trailhead &trail, const int_map_t &map, trailhead_list_t &trailends, xy_pos_t dim)
{
    //loop through all the possible directions
    for (const auto &trail_dir : trail_dirs) {
        xy_pos_t new_pos = trail.pos + trail_dir;
        if (is_pos_on_map(new_pos, dim) && map.at(new_pos.second).at(new_pos.first) == trail.height + 1) {
            if (map.at(new_pos.second).at(new_pos.first) == 9) {
                trailhead new_trail_end{ .pos = new_pos, .height = 9 };
                if (part_2_t || !vector_contains(trailends, new_trail_end)) {
                    trailends.push_back(new_trail_end);
                }
                continue;
            } else {
                trace_trailhead<part_2_t>(trailhead{ .pos = new_pos, .height = trail.height + 1 }, map, trailends, dim);
            }
        }
    }
}

int main()
{
    const auto map = read_file();

    const auto trailheads = scan_for_trailheads(map);

    xy_pos_t dim = xy_pos_t{ map.at(0).size(), map.size() };

    int score_part1{ 0 };
    int score_part2{ 0 };

    for (const auto &trailhead : trailheads) {
        trailhead_list_t trailends_part1;
        trace_trailhead<false>(trailhead, map, trailends_part1, dim);
        score_part1 += trailends_part1.size();

        trailhead_list_t trailends_part2;
        trace_trailhead<true>(trailhead, map, trailends_part2, dim);
        score_part2 += trailends_part2.size();
    }

    std::cout << score_part1 << '\n';
    std::cout << score_part2 << '\n';
}