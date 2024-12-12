
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
    bool is_grouped{ false };
    xy_pos_t pos{ 0, 0 };
};

using plot_row_t = std::vector<garden_plot>;
using plot_map_t = std::vector<plot_row_t>;

bool is_pos_on_map(xy_pos_t pos, xy_pos_t dim)
{
    return !(pos.first < 0 || pos.second < 0 || pos.first >= dim.first || pos.second >= dim.second);
}

static constexpr std::array<xy_pos_t, 4> grow_dirs = { xy_pos_t{ 1, 0 }, xy_pos_t{ 0, 1 }, xy_pos_t{ -1, 0 },
                                                       xy_pos_t{ 0, -1 } };

plot_map_t read_file()
{
    std::ifstream infile("input.txt");
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

void grow_group(plot_map_t &plot_map, std::vector<garden_plot> &grouped_plots, xy_pos_t pos, xy_pos_t dim, int & fence_length)
{
    plot_map.at(pos.second).at(pos.first).is_grouped = true;
    grouped_plots.push_back(plot_map.at(pos.second).at(pos.first));
    char current_plant = plot_map.at(pos.second).at(pos.first).plant_type;
    //check all the directions we can grow
    for (const auto &grow_dir : grow_dirs) {
        xy_pos_t new_pos = pos + grow_dir;
        if (is_pos_on_map(new_pos, dim) && plot_map.at(new_pos.second).at(new_pos.first).is_grouped == false &&
            plot_map.at(new_pos.second).at(new_pos.first).plant_type == current_plant) {
            grow_group(plot_map, grouped_plots, new_pos, dim, fence_length);
        }
        //if we go off the map or its a different plant, we can sum fence
        if (!is_pos_on_map(new_pos, dim) || plot_map.at(new_pos.second).at(new_pos.first).plant_type != current_plant) {
            fence_length++;
        }
    }
}

int group_plots(plot_map_t &plot_map)
{
    xy_pos_t dim{ plot_map.at(0).size(), plot_map.size() };
    std::vector<std::vector<garden_plot>> grouped_plots;
    std::vector<int> fence_lengths;
    //find first one that is not grouped
    size_t y{ 0 };
    for (auto &plot_row : plot_map) {
        plot_row_t::iterator itr;
        while ((itr = std::find_if(plot_row.begin(), plot_row.end(), [](auto &a) { return a.is_grouped == false; })) !=
               plot_row.end()) {
            size_t x = std::distance(plot_row.begin(), itr);
            grouped_plots.emplace_back();
            fence_lengths.push_back(0);
            grow_group(plot_map, grouped_plots.back(), xy_pos_t{ x, y }, dim, fence_lengths.back());
            //std::cout << fence_lengths.back() << '\n';
        }
        y++;
    }

    int price_sum {0};
    for (size_t i = 0; i < grouped_plots.size(); i++) {
        int area = grouped_plots.at(i).size();
        int perimeter = fence_lengths.at(i);
        price_sum += area * perimeter;
    }

    return price_sum;
}

int main()
{
    auto plot_map = read_file();

    int price_sum = group_plots(plot_map);
    std::cout << price_sum << '\n';
}