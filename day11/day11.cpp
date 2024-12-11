
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <cmath>
#include <tuple>
#include <ranges>
#include <iostream>

using num_type_t = long;
using int_row_t = std::list<num_type_t>;

int_row_t read_file()
{
    std::ifstream infile("input.txt");

    std::string line; 
    std::getline(infile, line);
    std::istringstream ss(line);
    int_row_t row;

    num_type_t num{ 0 };
    while (ss >> num) {
        row.push_back(num);
    }    

    return row;
}

bool is_even_num_digits(num_type_t num)
{
    //find the log10 of b
    double log_num = std::log10(static_cast<double>(num));
    //round it up
    double round_up = std::floor(log_num) + 1.0;
    //get the num digits
    num_type_t num_digits = std::lround(round_up);
    return num_digits % 2 == 0;
}

std::tuple<num_type_t, num_type_t> split_num(num_type_t num)
{
    //find the log10 of b
    double log_num = std::log10(static_cast<double>(num));
    //round it up
    double round_up = std::floor(log_num) + 1.0;
    //get the num digits
    double scale = std::pow(10, round_up / 2);
    long lscale = std::lround(scale);
    num_type_t num1 = num / lscale;
    num_type_t num2 = num % lscale;
    return {num1, num2}; 
}

void stones_tick(int_row_t & stones)
{
    for (auto itr = stones.begin(); itr != stones.end(); itr++) {
        //if stone is 0 it becomes 1
        //if even number of digits it splits
        //else the stone is multiplied by 2024
        if ((*itr) == 0) {
            (*itr) = 1;
        } else if (is_even_num_digits((*itr))) {
            auto [num1, num2] = split_num(*itr);
            (*itr) = num1;
            stones.insert(std::next(itr, 1), num2);
            //we need to advance the itr because we added an element
            itr++;
        } else {
            (*itr) *= 2024;
        }        
    }
}

int main()
{
    auto stones_part1 = read_file();
    auto stones_part2 = stones_part1;

    for (int i : std::views::iota(0, 25))
        stones_tick(stones_part1);
    
    std::cout << stones_part1.size() << '\n';

    // int_row_t one_stone;
    // one_stone.splice(one_stone.begin(), stones_part2, stones_part2.begin(), std::next(stones_part2.begin(), 1));

    // for (int i : std::views::iota(0, 75))
    //     stones_tick(one_stone);
    
    // std::cout << stones_part2.size() << '\n';
}