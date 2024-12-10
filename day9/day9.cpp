
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <list>

struct disk_element {
    int id{ -1 };
    int size{ 0 };
};

using int_row_t = std::vector<int>;
using int_list_t = std::list<int>;
using element_list_t = std::list<disk_element>;

element_list_t read_file()
{
    std::ifstream infile("input.txt");

    std::string line;
    std::getline(infile, line);
    std::istringstream ss(line);
    element_list_t row;

    bool is_empty{ false };
    int id{ 0 };
    char value{ 0 };
    while (ss >> value) {
        if (is_empty) {
            row.push_back(disk_element{ .id = -1, .size = value - '0' });
            is_empty = false;
        } else {
            row.push_back(disk_element{ .id = id, .size = value - '0' });
            id++;
            is_empty = true;
        }
    }

    return row;
}

int_row_t disk_map_to_blocks(const element_list_t &disk_map)
{
    int_row_t blocks;

    for (auto num : disk_map) {
        for (int i = 0; i < num.size; i++) {
            blocks.push_back(num.id);
        }
    }

    return blocks;
}

void compact_blocks_part1(int_row_t &blocks)
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

void compact_files_part2(element_list_t &elements)
{
    //find file with max id
    auto el_ritr = std::find_if(elements.rbegin(), elements.rend(), [](auto a) { return a.id != -1; });
    int id = (*el_ritr).id;

    //loop backwards
    for (int i = id; i >= 0; i--) {
        el_ritr = std::find_if(el_ritr, elements.rend(), [i](auto a) { return a.id == i; });
        size_t el_index = elements.size() - (std::distance(elements.rbegin(), el_ritr) + 1);
        auto el_itr = std::next(elements.begin(), el_index);

        auto empty_itr = std::find_if(elements.begin(), elements.end(),
                                      [el_itr](auto a) { return a.id == -1 && a.size >= (*el_itr).size; });
        size_t empty_index = std::distance(elements.begin(), empty_itr);

        //only move it if the empty is before the element
        if (el_index < empty_index || empty_itr == elements.end())
            continue;

        //move the element
        int size_diff = (*empty_itr).size - (*el_itr).size;
        std::iter_swap(empty_itr, el_itr);

        //adjust the empty size
        (*el_itr).size = (*empty_itr).size;

        //if size_diff is greater than zero we need to
        //insert an empty element after where the element was moved to
        if (size_diff > 0) {
            elements.insert(std::next(empty_itr, 1), disk_element{ .id = -1, .size = size_diff });
        }
    }
}

long calc_checksum(const int_row_t &blocks)
{
    int pos{ 0 };
    long checksum{ 0 };
    for (auto num : blocks) {
        if (num != -1)
            checksum += num * pos;
        pos++;
    }
    return checksum;
}

long calc_checksum(const element_list_t &disk_map)
{
    int pos{ 0 };
    long checksum{ 0 };
    for (const auto &element : disk_map) {
        if (element.id != -1) {
            for (int i = 0; i < element.size; i++) {
                checksum += element.id * pos;
                pos++;
            }
        } else {
            pos += element.size;
        }
    }
    return checksum;
}

int main()
{
    //part1
    auto disk_map = read_file();

    auto blocks = disk_map_to_blocks(disk_map);

    compact_blocks_part1(blocks);

    long checksum = calc_checksum(blocks);

    std::cout << checksum << '\n';

    //part2
    compact_files_part2(disk_map);

    long checksum2 = calc_checksum(disk_map);

    std::cout << checksum2 << '\n';
}