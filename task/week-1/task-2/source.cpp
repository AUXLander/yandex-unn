#define DEBUG_rW1T2

#ifndef DEBUG_rW1T2
#define RELEASE
#endif

#include <iostream>
#include <stack>
#include <map>
#include <functional>

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

			return static_cast<T>(NULL);
		}

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

class rW1T2 : public Task
{
	enum class rule
	{
		open,
		close
	};

	template<typename V>
	struct symbol
	{
		const V code;

		symbol(const V code) : code(code) {}

		struct less
		{
			bool operator()(const symbol<V>& left, const symbol<V>& right) const
			{
				return left.code < right.code;
			}
		};

		bool operator==(const symbol<V>& other) const
		{
			return this->code == other.code;
		}
	};

	const std::map<symbol<char>, const rule, symbol<char>::less> ruleset =
	{
		{ '(', rule::open }, { ')', rule::close },
		{ '[', rule::open }, { ']', rule::close },
		{ '{', rule::open }, { '}', rule::close },
	};

	const std::map<symbol<char>, symbol<char>, symbol<char>::less> closerules =
	{
		{'(', ')'},
		{'[', ']'},
		{'{', '}'},
	};

	void test(Test* const reference) override final {};
	void main(std::istream& input, std::ostream& output) override final;

public:

	rW1T2() : Task(nullptr) {}
	rW1T2(Test* const reference) : Task(reference) {}

};

void rW1T2::main(std::istream& input, std::ostream& output)
{
	size_t index = 0;
	std::stack<symbol<char>> stack;
	std::istream_iterator<char, char> istream(input);

	char cnext = next<char>(istream);

	while ((cnext != '\n') && (cnext != '\0'))
	{
		const symbol<char> snext(cnext);

		const auto alphabet_item = ruleset.find(snext);

		// check is rule exist for this symbol
		if (alphabet_item != ruleset.end())
		{
			// select rule: is that symbol opening or closing
			if (alphabet_item->second == rule::close)
			{
				if (stack.empty() == false)
				{
					const auto closerule = closerules.find(stack.top());

					// the close rule is exist
					if (closerule != closerules.end())
					{
						const symbol<char>& sclose = closerule->second;

						if (sclose == snext)
						{
							stack.pop();
							index++;
						}
						else
						{
							// ERROR: wrong closing symbol!
							output << index << std::endl;

							return;
						}
					}
					else
					{
						// ERROR: closing error is not exist!
						output << index << std::endl;

						return;
					}
				}
				else
				{
					// ERROR: closing not opened!
					output << index << std::endl;

					return;
				}
			}
			else
			{
				stack.push(snext);
				index++;
			}
		}
		else
		{
			// ERROR: wrong symbol!
			output << index << std::endl;

			return;
		}

		cnext = next(istream);
	}

	if (stack.empty() == false)
	{
		// ERROR: sequence is not closed!
		output << index << std::endl;

		return;
	}

	output << "CORRECT" << std::endl;
}

#ifndef DEBUG_rW1T2
int main(int argc, char* argv[])
{
	rW1T2 task;

	task.run(std::cin, std::cout);

	return 0;
}
#endif // !DEBUG