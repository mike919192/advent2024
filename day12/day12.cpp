
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

using xy_pos_t = std::pair<int, int>;

struct garden_plot {
    char plant_type{ 0 };
    xy_pos_t pos{ 0, 0 };
    bool is_grouped{ false };
};

using plot_row_t = std::vector<garden_plot>;
using plot_map_t = std::vector<plot_row_t>;

enum class fence_direction { horizontal, vertical };

struct fence_segment {
    xy_pos_t start{ 0, 0 };
    xy_pos_t end{ 0, 0 };
    fence_direction direction{ fence_direction::horizontal };
    bool is_traced{ false };
};

using fence_row_t = std::vector<fence_segment>;
using fence_map_t = std::vector<fence_row_t>;

bool is_pos_on_map(xy_pos_t pos, xy_pos_t dim)
{
    return !(pos.first < 0 || pos.second < 0 || pos.first >= dim.first || pos.second >= dim.second);
}

static constexpr std::array<xy_pos_t, 4> grow_dirs = { xy_pos_t{ 1, 0 }, xy_pos_t{ 0, 1 }, xy_pos_t{ -1, 0 },
                                                       xy_pos_t{ 0, -1 } };

plot_map_t read_file()
{
    std::ifstream infile("test.txt");
    plot_map_t csv_rows;
    int y{ 0 };

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        plot_row_t row;
        int x{ 0 };

        char value{ 0 };
        while (ss >> value) {
            row.push_back(garden_plot{ .plant_type = value, .pos = xy_pos_t{ x, y } });
            x++;
        }

        csv_rows.push_back(std::move(row));
        y++;
    }

    return csv_rows;
}

int count_fence_sides(fence_row_t &fence_group)
{
    //start with first fence segment that is not traced
    auto find_beginning = [](const fence_segment &a) { return !a.is_traced; };
    fence_row_t::iterator itr1;
    int count_sides{ 0 };

    while ((itr1 = std::find_if(fence_group.begin(), fence_group.end(), find_beginning)) != fence_group.end()) {
        (*itr1).is_traced = true;
        fence_row_t::iterator itr2;
        bool begin_to_end{ true };
        count_sides++;

        auto connect_fence = [&itr1, &begin_to_end](const fence_segment &a) {
            if (begin_to_end)
                return (!a.is_traced) && ((*itr1).end == a.start || (*itr1).end == a.end);
            else
                return (!a.is_traced) && ((*itr1).start == a.start || (*itr1).start == a.end);
        };

        while ((itr2 = std::find_if(fence_group.begin(), fence_group.end(), connect_fence)) != fence_group.end()) {
            (*itr2).is_traced = true;
            begin_to_end = ((*itr1).end == (*itr2).start) || ((*itr1).start == (*itr2).start);
            if ((*itr1).direction != (*itr2).direction)
                count_sides++;
            itr1 = itr2;
        }
    }

    return count_sides;
}

void grow_group(plot_map_t &plot_map, plot_row_t &plot_group, xy_pos_t pos, xy_pos_t dim, fence_row_t &fence_group)
{
    plot_map.at(pos.second).at(pos.first).is_grouped = true;
    plot_group.push_back(plot_map.at(pos.second).at(pos.first));
    char current_plant = plot_map.at(pos.second).at(pos.first).plant_type;
    //check all the directions we can grow
    for (const auto &grow_dir : grow_dirs) {
        xy_pos_t new_pos = pos + grow_dir;
        if (is_pos_on_map(new_pos, dim) && plot_map.at(new_pos.second).at(new_pos.first).is_grouped == false &&
            plot_map.at(new_pos.second).at(new_pos.first).plant_type == current_plant) {
            grow_group(plot_map, plot_group, new_pos, dim, fence_group);
        }
        //if we go off the map or its a different plant, we can sum fence
        if (!is_pos_on_map(new_pos, dim) || plot_map.at(new_pos.second).at(new_pos.first).plant_type != current_plant) {
            if (grow_dir == grow_dirs.at(0)) {
                fence_group.push_back(fence_segment{
                    .start = new_pos, .end = new_pos + xy_pos_t{ 0, 1 }, .direction = fence_direction::vertical });
            } else if (grow_dir == grow_dirs.at(1)) {
                fence_group.push_back(fence_segment{
                    .start = new_pos, .end = new_pos + xy_pos_t{ 1, 0 }, .direction = fence_direction::horizontal });
            } else if (grow_dir == grow_dirs.at(2)) {
                fence_group.push_back(fence_segment{
                    .start = pos, .end = pos + xy_pos_t{ 0, 1 }, .direction = fence_direction::vertical });
            } else {
                fence_group.push_back(fence_segment{
                    .start = pos, .end = pos + xy_pos_t{ 1, 0 }, .direction = fence_direction::horizontal });
            }
        }
    }
}

int group_plots(plot_map_t &plot_map)
{
    xy_pos_t dim{ plot_map.at(0).size(), plot_map.size() };
    plot_map_t plot_groups;
    fence_map_t fence_groups;
    //find first one that is not grouped
    size_t y{ 0 };
    for (auto &plot_row : plot_map) {
        plot_row_t::iterator itr;
        while ((itr = std::find_if(plot_row.begin(), plot_row.end(), [](auto &a) { return a.is_grouped == false; })) !=
               plot_row.end()) {
            size_t x = std::distance(plot_row.begin(), itr);
            plot_groups.emplace_back();
            fence_groups.emplace_back();
            grow_group(plot_map, plot_groups.back(), xy_pos_t{ x, y }, dim, fence_groups.back());
            //std::cout << fence_lengths.back() << '\n';
        }
        y++;
    }

    int price_sum_part1{ 0 };
    int price_sum_part2{ 0 };
    for (size_t i = 0; i < plot_groups.size(); i++) {
        int area = plot_groups.at(i).size();
        int perimeter = fence_groups.at(i).size();
        int count_sides = count_fence_sides(fence_groups.at(i));
        price_sum_part1 += area * perimeter;
        price_sum_part2 += area * count_sides;
    }

    return price_sum_part2;
}

int main()
{
    auto plot_map = read_file();

    int price_sum = group_plots(plot_map);
    std::cout << price_sum << '\n';
}