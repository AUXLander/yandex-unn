#include <iterator>
#include <iostream>

#include <cstddef>
#include <utility>
#include <algorithm>
#include <functional>
#include <vector>
#include <limits>
#include <string>
#include <deque>

class Test {};

class Task
{
protected:
    size_t m_index;

    Test* const m_reference;

    inline void reset() { m_index = 0; }

    template<typename T>
    T next(std::istream_iterator<T, char>& input)
    {
        if (m_index > 0)
        {
            ++input;
        }

        m_index++;

        if (input == std::istream_iterator<T, char>())
        {
            if constexpr (std::is_same<T, char>())
            {
                return '\0';
            }
            else
            {
                return static_cast<T>(NULL);
            }
        }

        return *input;
    }

    virtual void test(Test* const reference) = 0;
    virtual void main(std::istream& input, std::ostream& output) = 0;

public:
    explicit Task(Test* const reference) : m_index(0), m_reference(reference) {};

    Task(Task&&) = delete;
    Task(const Task&) = delete;

    virtual ~Task() {};

    void selftest()
    {
        if (m_reference != nullptr)
        {
            test(m_reference);
        }
    }

    void run(std::istream& input, std::ostream& output)
    {
        reset();
        main(input, output);
    }
};

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

    void push_back(const int value)
    {
        while (!d.empty() && d.back().first <= value)
        {
            d.pop_back();
        }

        d.emplace_back(value, rIdx);

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

class W2T1 : public Task
{
    void test(Test* const reference) override final {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    W2T1() : Task(nullptr) {}
    explicit W2T1(Test* const reference) : Task(reference) {}
};

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

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    W2T1 task;

    task.run(std::cin, std::cout);

    return 0;
}
