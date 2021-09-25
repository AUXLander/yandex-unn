#pragma once
#include <istream>
#include <ostream>

class Task
{
protected:
	size_t m_index;

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

public:
	Task() : m_index(0) {};

	virtual void run(std::istream& input, std::ostream& output) = 0;
};