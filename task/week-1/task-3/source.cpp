#define DEBUG_rW1T3

#ifndef DEBUG_rW1T3
#define RELEASE
#endif

#include <iostream>
#include <iterator>
#include <string>
#include <deque>
#include <stack>
#include <algorithm>

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

class rW1T3 : public Task
{
    int clamp(const int min, const int value, const int max) 
    { 
        return std::min<int>(max, std::max(min, value));
    };

    void test(Test* const reference) override final {}
    void main(std::istream& input, std::ostream& output) override final;

public:
    rW1T3() : Task(nullptr) {}
    explicit rW1T3(Test* const reference) : Task(reference) {}
};

void rW1T3::main(std::istream& input, std::ostream& output)
{
    std::deque<std::deque<int>> matrixA;
    std::deque<std::deque<int>> matrixB;

    std::istream_iterator<int, char> istream(input);

    const int lengthA = next(istream);
    const int lengthB = next(istream);
    const int size = next(istream);

    for (int index = 0; index < lengthA; ++index)
    {
        auto& column = matrixA.emplace_back();

        for (int position = 0; position < size; ++position)
        {
            column.push_back(next(istream));
        }
    }

    for (int index = 0; index < lengthB; ++index)
    {
        auto& column = matrixB.emplace_back();

        for (int position = 0; position < size; ++position)
        {
            column.push_back(next(istream));
        }
    }

    const auto element = [&matrixA, &matrixB](const int matrixIdxA,
        const int matrixIdxB,
        const int elementIdx)
    {
        return std::max(matrixA[matrixIdxA][elementIdx], matrixB[matrixIdxB][elementIdx]);
    };

    struct MarkUp
    {
        int left;
        int absolute;
        int right;
    };

    union
    {
        MarkUp minimum;
        int       index_access[3];
    };

    std::string outstring;

    const int mid = size / 2U;
    const int limit_offset = (size / 2U) + 1;

    const int lengthQ = next(istream);

    for (int index = 0; index < lengthQ; ++index)
    {
        const int matrixIdxA = next(istream) - 1U;
        const int matrixIdxB = next(istream) - 1U;

        int elementIdx = mid;

        minimum.absolute = element(matrixIdxA, matrixIdxB, elementIdx);

        minimum.left = minimum.absolute;
        minimum.right = minimum.absolute;

        for (int offset = 1; offset < limit_offset; ++offset)
        {
            for (int sign = -1; sign < 2; sign += 2)
            {
                const int position = clamp(0, mid - sign * offset, size - 1U);
                const int locmin = element(matrixIdxA, matrixIdxB, position);

                index_access[1U + sign] = locmin;

                if (locmin < minimum.absolute)
                {
                    minimum.absolute = locmin;
                    elementIdx = position;
                }
            }

            if ((minimum.left > minimum.absolute) && (minimum.absolute < minimum.right))
            {
                break;
            }
        }

        outstring += '\n';
        outstring += std::to_string(elementIdx + 1U);
    }

    output << outstring.erase(0, 1);
}

#ifndef DEBUG_rW1T3

int main(int argc, char* argv[])
{
    rW1T3 task;

    task.run(std::cin, std::cout);

    return 0;
}

#endif // !DEBUG
