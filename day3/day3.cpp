
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <iostream>

std::string readFile()
{
    std::ifstream infile("input.txt");
    std::stringstream buffer;
    buffer << infile.rdbuf();
    return buffer.str();
}

int main()
{
    const std::string allText = readFile();

    std::regex word_regex("mul\\((\\d+),(\\d+)\\)|do\\(\\)|don't\\(\\)");
    auto words_begin = std::sregex_iterator(allText.begin(), allText.end(), word_regex);
    auto words_end = std::sregex_iterator();

    int sum{ 0 };
    bool enable{ true };

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        //std::cout << match_str << '\n';

        if (match_str == "do()") {
            //std::cout << "enable\n";
            enable = true;
        } else if (match_str == "don't()") {
            //std::cout << "disable\n";
            enable = false;
        } else {
            // std::cout << match[1].str() << "\n";
            // std::cout << match[2].str() << "\n";

            if (enable == true && match.size() >= 3) {
                int num1 = std::stoi(match[1].str());
                int num2 = std::stoi(match[2].str());

                sum += num1 * num2;
            }
        }
    }

    std::cout << sum << '\n';
}