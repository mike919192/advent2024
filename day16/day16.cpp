
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "advent.h"

struct cell {
    bool has_wall{ false };
};

struct endpoint {
    xy_pos_t pos{ 0, 0 };
};

using maze_row_t = std::vector<cell>;
using maze_map_t = std::vector<maze_row_t>;

std::tuple<maze_map_t, endpoint, endpoint> read_file()
{
    std::ifstream infile("test.txt");
    maze_map_t csv_rows;
    endpoint start;
    endpoint end;
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
                start = endpoint{{x, y}};
                row.push_back(cell{});
                break;
            case 'E':
                end = endpoint{{x, y}};
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

int main()
{
    auto [map, start, end] = read_file();
}