#include <iostream>
#include <vector>
#include <array>
#include <stack>
#include <cmath>
#include <deque>
#include <utility>
#include <string>

int main(int, char*[])
{
    struct Element
    {
        int value;

        std::pair<size_t, Element*> more;
        std::pair<size_t, Element*> less;

        Element(std::stack<int>& stack, 
                std::pair<size_t, Element*>& more, 
                std::pair<size_t, Element*>& less)
            : value(stack.top()), more(more), less(less) {}
    };

    int length;
    std::string result;
    std::stack<int> stack;
    std::deque<Element> nodes;

    std::pair<size_t, Element*> more(0, nullptr);
    std::pair<size_t, Element*> less(0, nullptr);

    bool dir;

    const auto select = [&dir](const std::pair<size_t, Element*>& fdirmax, 
                               const std::pair<size_t, Element*>& tdirmax)
    {
        if (dir == false)
        {
            return fdirmax.second;
        }
        else
        {
            return tdirmax.second;
        }
    };

    std::cin >> length;

    for (int i = 0; i < length; ++i)
    {
        int value;

        std::cin >> value;

        stack.push(value);
    }

    while (!stack.empty())
    {
        std::pair<size_t, Element*> tmore(0, nullptr);
        std::pair<size_t, Element*> tless(0, nullptr);

        for (size_t index = 0; index < nodes.size(); ++index)
        {
            Element& node = nodes[nodes.size() - 1 - index];

            if (node.value > stack.top())
            {
                if (node.less.first + 1 > tmore.first)
                {
                    tmore.first = node.less.first + 1;
                    tmore.second = &node;
                }
            }

            if (node.value < stack.top())
            {
                if (node.more.first + 1 > tless.first)
                {
                    tless.first = node.more.first + 1;
                    tless.second = &node;
                }
            }
        }

        nodes.push_back({ stack, tmore, tless });

        if (nodes.back().more.first >= more.first)
        {
            more.first = nodes.back().more.first;
            more.second = &nodes.back();
        }

        if (nodes.back().less.first >= less.first)
        {
            less.first = nodes.back().less.first;
            less.second = &nodes.back();
        }

        stack.pop();
    }

    dir = (more.first < less.first);

    Element* pointer = select(more, less);

    while (pointer != nullptr)
    {
        result += ' ' + std::to_string(pointer->value);

        pointer = select(pointer->more, pointer->less);

        dir = !dir;
    }

    if (length > 0)
    {
        std::cout << result.substr(1, -1);
    }

    return 0;
}
