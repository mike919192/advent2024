
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

using int_row_t = std::vector<int>;

int_row_t read_file()
{
    std::ifstream infile("input.txt");

    std::string line; 
    std::getline(infile, line);
    std::istringstream ss(line);
    int_row_t row;

    char value{ 0 };
    while (ss >> value) {
        row.push_back(value - '0');
    }    

    return row;
}

int_row_t disk_map_to_blocks(const int_row_t & disk_map)
{
    int_row_t blocks;

    bool is_empty_space {false};
    int id { 0 };

    for (auto num : disk_map) {
        if (is_empty_space) {
            for (int i = 0; i < num; i++)
                blocks.push_back(-1);

        } else {
            for (int i = 0; i < num; i++)
                blocks.push_back(id);
            id++;
        }

        is_empty_space = !is_empty_space;
    }

    return blocks;
}

void compact_blocks(int_row_t & blocks)
{
    while (true) {
        //find first empty spot
        auto itr1 = std::find(blocks.begin(), blocks.end(), -1);

        //find last block
        auto itr2 = std::find_if(blocks.rbegin(), blocks.rend(), [](auto x) { return x != -1; });

        //if first empty space is after last block we are done
        size_t index1 = std::distance(blocks.begin(), itr1);
        size_t index2 = blocks.size() - (std::distance(blocks.rbegin(), itr2) + 1);
        if (index2 < index1)
            break;

        //swap them
        std::iter_swap(itr1, itr2);
    }
}

long calc_checksum(const int_row_t & blocks)
{
    int pos {0};
    long checksum {0};
    for (auto num : blocks) {
        if (num == -1)
            break;
        checksum += num * pos;
        pos++;
    }
    return checksum;
}

int main()
{
    const auto disk_map = read_file();

    auto blocks = disk_map_to_blocks(disk_map);

    compact_blocks(blocks);

    long checksum = calc_checksum(blocks);

    std::cout << checksum << "\n";
}