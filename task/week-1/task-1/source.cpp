#define DEBUG_rW1T1

#ifndef DEBUG_rW1T1
#define RELEASE
#endif


#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <stack>
#include <cmath>

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

class rW1T1 : public Task
{
	void test(Test* const reference) override final {};
	void main(std::istream& input, std::ostream& output) override final;

public:

	rW1T1() : Task(nullptr) {}
	rW1T1(Test* const reference) : Task(reference) {}

};

struct Node
{
	int value;

	struct NodeCounter
	{
		size_t count;
		Node* pointer;

		NodeCounter(const size_t count, Node* const pointer) : count(count), pointer(pointer) {}

		NodeCounter& copy(const size_t Count, Node* const Pointer)
		{
			count = Count;
			pointer = Pointer;

			return *this;
		}
	};

	NodeCounter more;
	NodeCounter less;

	Node(int v, size_t cm, Node* const pmore, size_t cl, Node* const pless)
		: value(v), more(cm, pmore), less(cl, pless)
	{
	}
};

void rW1T1::main(std::istream& input, std::ostream& output)
{
	std::stack<int> stack;
	std::istream_iterator<int, char> istream(input);

	const int length = next(istream);

	if (length > 0)
	{
		for (int i = 0; i < length; ++i)
		{
			stack.push(next(istream));
		}

		std::list<Node> nodes;

		Node::NodeCounter max_more(0, nullptr);
		Node::NodeCounter max_less(0, nullptr);

		while (stack.empty() == false)
		{
			const auto top = stack.top();

			Node::NodeCounter loc_max_more(0, nullptr);
			Node::NodeCounter loc_max_less(0, nullptr);

			for (auto it = nodes.rbegin(); it != nodes.rend(); ++it)
			{
				if ((it->value >= top) && (it->less.count + 1 > loc_max_more.count))
				{
					loc_max_more.copy(it->less.count + 1, &(*it));
				}

				if ((it->value <= top) && (it->more.count + 1 > loc_max_less.count))
				{
					loc_max_less.copy(it->more.count + 1, &(*it));
				}
			}

			auto& node = nodes.emplace_back(top, loc_max_more.count, loc_max_more.pointer, loc_max_less.count, loc_max_less.pointer);

			if (node.more.count >= max_more.count)
			{
				max_more.copy(node.more.count, &node);
			}

			if (node.less.count >= max_less.count)
			{
				max_less.copy(node.less.count, &node);
			}

			stack.pop();
		}

		std::string outstring;

		bool direction = (max_more.count < max_less.count);

		Node* node = !direction ? max_more.pointer : max_less.pointer;

		while (node != nullptr)
		{
			outstring += ' ' + std::to_string(node->value);

			node = direction ? node->less.pointer : node->more.pointer;

			direction = !direction;
		}

		output << outstring.erase(0, 1);
	}
}

#ifndef DEBUG_rW1T1
int main(int argc, char* argv[])
{
	rW1T1 task;

	task.run(std::cin, std::cout);

	return 0;
}
#endif // !DEBUG