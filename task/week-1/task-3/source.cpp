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

	matrixA.resize(lengthA);
	matrixB.resize(lengthB);

	for (int index = 0; index < lengthA; ++index)
	{
		auto& column = matrixA[index];

		column.resize(size);

		for (int position = 0; position < size; ++position)
		{
			column[position] = next(istream);
		}
	}

	for (int index = 0; index < lengthB; ++index)
	{
		auto& column = matrixB[index];

		column.resize(size);

		for (int position = 0; position < size; ++position)
		{
			column[position] = next(istream);
		}
	}

	const auto pick = [&matrixA, &matrixB](const int matrixIdxA,
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
		int	   index_access[3];
	};

	const int lengthQ = next(istream);

	std::string result;

	for (int index = 0; index < lengthQ; ++index)
	{
		if (index > 0)
		{
			result += '\n';
		}

		const int matrixIdxA = next(istream) - 1U;
		const int matrixIdxB = next(istream) - 1U;

		int left = 0;
		int right = size - 1;

		while (std::abs(right - left) > 1U)
		{
			const int middle = (left + right) / 2U;

			const int lquad = (left + middle) / 2U;
			const int rquad = (right + middle) / 2U;

			const int xx = pick(matrixIdxA, matrixIdxB, middle);

			const int lx = pick(matrixIdxA, matrixIdxB, lquad);
			const int rx = pick(matrixIdxA, matrixIdxB, rquad);

			if (lx < xx)
			{
				right = middle;
			}
			else if (rx < xx)
			{
				left = middle;
			}
			else
			{
				left = lquad;
				right = rquad;
			}
		}

		const int zleft = pick(matrixIdxA, matrixIdxB, left);
		const int zright = pick(matrixIdxA, matrixIdxB, right);

		if (zleft < zright)
		{
			result += std::to_string(left + 1U);
		}
		else
		{
			result += std::to_string(right + 1U);
		}
	}

	output << result;
}

#ifndef DEBUG_rW1T3

int main(int argc, char* argv[])
{
    rW1T3 task;

    task.run(std::cin, std::cout);

    return 0;
}

#endif // !DEBUG
