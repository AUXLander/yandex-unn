#include "W2T1.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <utility>
#include <iterator>

struct Frame
{
    int lIdx, rIdx;
    std::deque<std::pair<int, int>> d;

    Frame() : lIdx(0), rIdx(0)
    {;}

    int get_max() const
    {
        return d.front().first;
    }

    void push_back(const int p_root)
    {
        while (!d.empty() && d.back().first <= p_root)
        {
            d.pop_back();
        }

        d.emplace_back(p_root, rIdx);

        ++rIdx;
    }

    void pop_front()
    {
        if (d.front().second == lIdx)
        {
            d.pop_front();
        }

        ++lIdx;
    }
};


void W2T1::test(Test* const reference)
{
    if( reference != nullptr)
    {
        reference->open(*this).input("10\n1 4 2 3 5 8 6 7 9 10\n12 R R L R R R L L L R L L ").expect("4 4 4 4 5 8 8 8 8 8 8 6 \n");
    }
}

void W2T1::main(std::istream& input, std::ostream& output)
{
    char ch;
    Frame window;
    int sizeN, sizeM;

    input >> sizeN;

    std::vector<int> data(sizeN);

    for (int i = 0; i < sizeN; ++i)
    {
        input >> data[i];
    }

    input >> sizeM;

    window.push_back(data[0]);

    for (int index = 0; index < sizeM; ++index)
    {
        input >> ch;

        if (ch == 'R')
        {
            window.push_back(data[window.rIdx]);
        }
        else
        {
            window.pop_front();
        }

        output << window.get_max() << ' ';
    }

    output << '\n';
}