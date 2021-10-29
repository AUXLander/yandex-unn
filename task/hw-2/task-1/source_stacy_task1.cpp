#include <iostream>
#include <utility>
#include <algorithm>
#include <limits>
#include <string>
#include <deque>

int left_index = 0, right_index = 0;

void push_back(std::deque<std::pair<int, int>>& window, const int value)
{
    while (!window.empty() && window.back().first <= value)
    {
        window.pop_back();
    }

    window.emplace_back(value, right_index);

    ++right_index;
}

void pop_front(std::deque<std::pair<int, int>>& window)
{
    if (window.front().second == left_index)
    {
        window.pop_front();
    }

    ++left_index;
}

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::deque<std::pair<int, int>> window;

    int NN, MM;
    char symbol;

    std::cin >> NN;

    std::deque<int> input(NN);

    for (auto& element : input)
    {
        std::cin >> element;
    }

    std::cin >> MM;

    push_back(window, input[0]);

    for (int count = 0; count < MM; ++count)
    {
        std::cin >> symbol;

        if (symbol == 'R')
        {
            push_back(window, input[right_index]);
        }
        else
        {
            pop_front(window);
        }

        std::cout << window.front().first << ' ';
    }

    std::cout << '\n';

    return 0;
}
