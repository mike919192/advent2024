
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <iostream>

//part1 constants
static constexpr std::array<char, 4> word_to_find{ 'X', 'M', 'A', 'S' };
static constexpr std::array<int, 8> cardinals_x{ 1, 1, 0, -1, -1, -1, 0, 1 };
static constexpr std::array<int, 8> cardinals_y{ 0, 1, 1, 1, 0, -1, -1, -1 };

//part 2 constants
// clang-format off
static constexpr std::array<std::array<char, 3>, 3> mas_mask{ { { '\xff', 0, '\xff' }, 
                                                                { 0, '\xff', 0 }, 
                                                                { '\xff', 0, '\xff' } } };

static constexpr std::array<std::array<char, 3>, 3> mas_x1{ { { 'S', 0, 'S' }, 
                                                              { 0, 'A', 0 }, 
                                                              { 'M', 0, 'M' } } };

static constexpr std::array<std::array<char, 3>, 3> mas_x2{ { { 'S', 0, 'M' }, 
                                                              { 0, 'A', 0 }, 
                                                              { 'S', 0, 'M' } } };

static constexpr std::array<std::array<char, 3>, 3> mas_x3{ { { 'M', 0, 'M' }, 
                                                              { 0, 'A', 0 }, 
                                                              { 'S', 0, 'S' } } };

static constexpr std::array<std::array<char, 3>, 3> mas_x4{ { { 'M', 0, 'S' }, 
                                                              { 0, 'A', 0 }, 
                                                              { 'M', 0, 'S' } } };
// clang-format on

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

bool is_xmas(const std::vector<std::vector<char>> &word_search, int start_x, int start_y, int dir_x, int dir_y)
{
    int dim_y = word_search.size();
    int dim_x = word_search.at(0).size();

    int x = start_x;
    int y = start_y;

    //assume start is already determined to be 'X'
    for (size_t i = 1; i < word_to_find.size(); i++) {
        x += dir_x;
        y += dir_y;

        if (x < 0 || y < 0 || x >= dim_x || y >= dim_y) {
            //reached edge without match
            return false;
        }

        if (word_search.at(y).at(x) != word_to_find.at(i)) {
            //letter doesnt match
            return false;
        }
    }

    return true;
}

bool is_mas_pattern(const std::vector<std::vector<char>> &word_search, int x, int y)
{
    //first off we need to make a smaller 3x3 array
    std::array<std::array<char, 3>, 3> area_to_check{ 0 };

    //copy and mask the values
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 3; i++) {
            area_to_check.at(j).at(i) = word_search.at(y + j - 1).at(x + i - 1) & mas_mask.at(j).at(i);
        }
    }

    //then check if any match
    return area_to_check == mas_x1 || area_to_check == mas_x2 || area_to_check == mas_x3 || area_to_check == mas_x4;
}

int main()
{
    const auto word_search = read_file();

    //part 1
    int xmas_count{ 0 };

    for (size_t y = 0; y < word_search.size(); y++) {
        for (size_t x = 0; x < word_search.at(y).size(); x++) {
            if (word_search.at(y).at(x) == word_to_find.at(0)) {
                for (size_t i = 0; i < cardinals_x.size(); i++) {
                    bool xmas_found = is_xmas(word_search, x, y, cardinals_x.at(i), cardinals_y.at(i));

                    if (xmas_found)
                        xmas_count++;
                }
            }
        }
    }

    std::cout << xmas_count << '\n';

    //part 2
    int mas_pattern_count{ 0 };

    for (size_t y = 2; y < word_search.size(); y++) {
        for (size_t x = 2; x < word_search.at(y).size(); x++) {
            if (word_search.at(y - 1).at(x - 1) == mas_x1.at(1).at(1)) {
                bool mas_pattern_found = is_mas_pattern(word_search, x - 1, y - 1);

                if (mas_pattern_found)
                    mas_pattern_count++;
            }
        }
    }

    std::cout << mas_pattern_count << '\n';
}