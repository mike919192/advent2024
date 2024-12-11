
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <cmath>
#include <tuple>
#include <ranges>
#include <iostream>
#include <future>
#include <vector>
#include <unordered_map>

using num_type_t = long;
struct stone {
    num_type_t value{0};
    int iteration {0};

    stone() = default;
    stone(num_type_t value) : value(value) {}
    stone(num_type_t value, int iteration) : value(value), iteration(iteration) {}
};


using stone_list_t = std::vector<stone>;

stone_list_t read_file()
{
    std::ifstream infile("input.txt");

    std::string line; 
    std::getline(infile, line);
    std::istringstream ss(line);
    stone_list_t row;

    num_type_t num{ 0 };
    while (ss >> num) {
        row.push_back(stone(num));
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

void stones_tick(stone_list_t & stones)
{
    for (auto itr = stones.begin(); itr != stones.end(); itr++) {
        //if stone is 0 it becomes 1
        //if even number of digits it splits
        //else the stone is multiplied by 2024
        if ((*itr).value == 0) {
            (*itr).value = 1;
            (*itr).iteration++;
        } else if (is_even_num_digits((*itr).value)) {
            auto [num1, num2] = split_num((*itr).value);
            (*itr).value = num1;
            (*itr).iteration++;
            stones.insert(std::next(itr, 1), stone(num2, (*itr).iteration));
            //we need to advance the itr because we added an element
            itr++;
        } else {
            (*itr).value *= 2024;
            (*itr).iteration++;
        }        
    }
}

num_type_t stones_eval(std::vector<stone> stones, num_type_t max_iter)
{
    std::unordered_map<num_type_t, std::pair<num_type_t, num_type_t>> umap;

    num_type_t sum {0};
    while(stones.size() > 0) {
        auto itr = stones.rbegin();
        //if we reached max_iter then remove it
        if ((*itr).iteration >= max_iter) {
            stones.pop_back();
            sum++;
            continue;
        }
        //check if it exists in the map
        auto itr2 = umap.find((*itr).value);
        if (itr2 != umap.end()) {
            auto saved = *itr2;
            (*itr).value = saved.second.first;
            (*itr).iteration++;
            if (saved.second.second != -1)
                stones.push_back(stone(saved.second.second, (*itr).iteration));
            continue;
        }

        //if stone is 0 it becomes 1
        //if even number of digits it splits
        //else the stone is multiplied by 2024
        if ((*itr).value == 0) {
            (*itr).value = 1;
            (*itr).iteration++;
            umap.insert({0, std::pair{1, -1}});
        } else if (is_even_num_digits((*itr).value)) {
            num_type_t prev = (*itr).value;
            auto [num1, num2] = split_num((*itr).value);
            (*itr).value = num1;
            (*itr).iteration++;
            stones.push_back(stone(num2, (*itr).iteration));
            umap.insert({prev, std::pair{num1, num2}});
        } else {
            num_type_t prev = (*itr).value;
            (*itr).value *= 2024;
            (*itr).iteration++;
            umap.insert({prev, std::pair{(*itr).value, -1}});
        }        
    }
    return sum;
}

int main()
{
    auto stones_part1 = read_file();
    auto stones_part2 = stones_part1;
    
    num_type_t sum_part1 = stones_eval(stones_part1, 25);

    std::cout << sum_part1 << '\n';

    std::vector<std::future<num_type_t>> futures;
    futures.reserve(8);
    num_type_t max_iter {25};
    num_type_t sum {0};

    for (auto & a_stone : stones_part2) {
        std::vector<stone> one_stone {a_stone};
        //sum += stones_eval(one_stone, max_iter);
        //std::cout << sum << '\n';
        //std::async(&stones_eval, one_stone, max_iter);
        futures.push_back(std::async(&stones_eval, one_stone, max_iter));

        if (futures.size() >= 8) {
            for (auto &fut : futures) {
                sum += fut.get();
                std::cout << sum << '\n';
            }
            futures.clear();
        }
    }

    std::cout << sum << '\n';

    // stone_list_t one_stone;
    // one_stone.splice(one_stone.begin(), stones_part2, stones_part2.begin(), std::next(stones_part2.begin(), 1));

    // for (int i : std::views::iota(0, 75))
    //     stones_tick(one_stone);
    
    // std::cout << stones_part2.size() << '\n';
}