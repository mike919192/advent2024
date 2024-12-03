
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <numeric>
#include <iostream>

int main()
{
    std::ifstream infile("input.txt");

    std::vector<int> list1, list2;

    //read in the file
    int a, b;
    while (infile >> a >> b) {
        list1.push_back(a);
        list2.push_back(b);
    }

    int length = list1.size();

    //create sort map
    std::vector<size_t> sort1, sort2;

    std::vector<int> tempList1 = list1;
    std::vector<int> tempList2 = list2;

    for (int i = 0; i < list1.size(); i++) {
        size_t index1 = std::min_element(tempList1.begin(), tempList1.end()) - tempList1.begin();
        sort1.push_back(index1);
        tempList1.at(index1) = std::numeric_limits<int>::max();

        size_t index2 = std::min_element(tempList2.begin(), tempList2.end()) - tempList2.begin();
        sort2.push_back(index2);
        tempList2.at(index2) = std::numeric_limits<int>::max();
    }

    std::vector<int> distances;

    for (int i = 0; i < list1.size(); i++) {
        long int distance = list1.at(sort1.at(i)) - list2.at(sort2.at(i));
        distances.push_back(std::abs(distance));
    }

    long int sum = std::accumulate(distances.begin(), distances.end(), 0);

    std::cout << sum << "\n";

    std::vector<int> score;

    for (int i = 0; i < list1.size(); i++) {
        int value = list1.at(i);

        int count{ 0 };
        for (int j = 0; j < list2.size(); j++) {
            if (value == list2.at(j)) {
                count++;
            }
        }
        score.push_back(value * count);
    }

    long int scoresum = std::accumulate(score.begin(), score.end(), 0);

    std::cout << scoresum << "\n";
}