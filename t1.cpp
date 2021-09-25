#include <vector>
#include <array>
#include <stack>
#include <cmath>

#include "test.h"
#include "t1.h"

struct Node
{
	int value;

	struct NodeCounter
	{
		size_t count;
		Node* pointer;

		NodeCounter(size_t count, Node* pointer) : count(count), pointer(pointer) {}
	};

	NodeCounter more;
	NodeCounter less;

	Node(int v, size_t cm, Node * const pmore, size_t cl, Node* const pless)
		: value(v), more(cm, pmore), less(cl, pless)
	{
	}
};

void T1::main(std::istream& input, std::ostream& output)
{
	std::stack<int> container;
	std::istream_iterator<int, char> istream(input);

	const int length = next(istream);

	if (length > 0)
	{
		for (int i = 0; i < length; ++i)
		{
			container.push(next(istream));
		}

		output << subrun(length, container);
	}
}

void T1::test(Test* const reference)
{
	reference->open(*this).input("10\n1 4 2 3 5 8 6 7 9 10\n").expect("1 4 2 8 6 7");
	reference->open(*this).input("5\n1 2 3 4 5\n").expect("1 2");
	reference->open(*this).input("1\n100\n").expect("100");

	reference->open(*this).input("12\n2 3 4 5 2 3 5 8 6 7 9 10\n").expect("2 3 2 8 6 7");

	reference->open(*this).input("15\n1 2 3 4 5 1 4 2 3 5 8 6 7 9 10\n").expect("1 2 1 4 2 8 6 7");
}


std::string T1::subrun(const int length, std::stack<int> stack)
{
	std::string output;

	std::vector<Node> nodes;

	nodes.reserve(stack.size());

	while (stack.empty() == false)
	{
		const auto top = stack.top();

		size_t max_count_more = 0;
		Node* max_more_ptr = nullptr;
		size_t max_count_less = 0;
		Node* max_less_ptr = nullptr;

		for (auto it = nodes.rbegin(); it != nodes.rend(); ++it)
		{
			if (it->value >= top)
			{
				if (it->less.count + 1 > max_count_more)
				{
					max_count_more = it->less.count + 1;
					max_more_ptr = &(*it);
				}
			}

			if (it->value <= top)
			{
				if (it->more.count + 1 > max_count_less)
				{
					max_count_less = it->more.count + 1;
					max_less_ptr = &(*it);
				}
			}
		}

		nodes.emplace_back(top, max_count_more, max_more_ptr, max_count_less, max_less_ptr);

		stack.pop();
	}

	int index_count_more = nodes.size() - 1;
	size_t max_count_more = nodes[index_count_more].more.count;

	int index_count_less = nodes.size() - 1;
	size_t max_count_less = nodes[index_count_less].less.count;

	for (int index = nodes.size() - 1; index >= 0; --index)
	{
		const auto& node = nodes[index];

		if (node.more.count > max_count_more)
		{
			max_count_more = node.more.count;
			index_count_more = index;
		}

		if (node.less.count > max_count_less)
		{
			max_count_less = node.less.count;
			index_count_less = index;
		}
	}

	bool direction;

	size_t index;

	if (max_count_more > max_count_less)
	{
		direction = false;
		index = index_count_more;
	}
	else
	{
		direction = true;
		index = index_count_less;
	}

	Node node = nodes[index];

	output += std::to_string(node.value);

	Node* next_ptr = direction ? node.less.pointer : node.more.pointer;

	direction = !direction;

	while (next_ptr != nullptr)
	{
		node = *next_ptr;

		output += ' ' + std::to_string(node.value);

		next_ptr = direction ? node.less.pointer : node.more.pointer;

		direction = !direction;
	}

	return output;
}
