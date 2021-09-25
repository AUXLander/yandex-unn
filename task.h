#pragma once
#include <istream>
#include <ostream>

class Test;

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

		return *input;
	}

	virtual void test(Test* const reference) = 0;
	virtual void main(std::istream& input, std::ostream& output) = 0;

public:
	Task(Test* const reference) : m_index(0), m_reference(reference) {};

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