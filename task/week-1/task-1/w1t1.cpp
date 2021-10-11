#include <vector>
#include <list>
#include <array>
#include <stack>
#include <cmath>
#include <deque>

#include "../../../test.h"
#include "w1t1.h"

struct Node
{
	int value;

	std::pair<size_t, Node*> more;
	std::pair<size_t, Node*> less;

	Node(std::stack<int>& stack, std::pair<size_t, Node*>& more, std::pair<size_t, Node*>& less)
		: value(stack.top()), more(more), less(less)
	{}
};

void W1T1::main(std::istream& input, std::ostream& output)
{
	int length;
	std::stack<int> stack;
	std::deque<Node> nodes;

	input >> length;

	for (int i = 0; i < length; ++i)
	{
		int value;

		input >> value;

		stack.push(value);
	}

	std::string outstring;

	std::pair<size_t, Node*> max_more(0, nullptr);
	std::pair<size_t, Node*> max_less(0, nullptr);

	while (!stack.empty())
	{
		std::pair<size_t, Node*> loc_max_more(0, nullptr);
		std::pair<size_t, Node*> loc_max_less(0, nullptr);

		for (size_t index = 0; index < nodes.size(); index++)
		{
			Node& node = nodes[nodes.size() - 1 - index];

			if (node.value >= stack.top())
			{
				if (node.less.first + 1 > loc_max_more.first)
				{
					loc_max_more.first = node.less.first + 1;
					loc_max_more.second = &node;
				}
			}

			if (node.value <= stack.top())
			{
				if (node.more.first + 1 > loc_max_less.first)
				{
					loc_max_less.first = node.more.first + 1;
					loc_max_less.second = &node;
				}
			}
		}

		Node& node = nodes.emplace_back(stack, loc_max_more, loc_max_less);

		if (node.more.first >= max_more.first)
		{
			max_more.first = node.more.first;
			max_more.second = &node;
		}

		if (node.less.first >= max_less.first)
		{
			max_less.first = node.less.first;
			max_less.second = &node;
		}

		stack.pop();
	}

	bool direction = (max_more.first < max_less.first);

	const auto select = [&direction](const std::pair<size_t, Node*>& fdirmax, const std::pair<size_t, Node*>& tdirmax)
	{
		if (direction == false)
		{
			return fdirmax.second;
		}
		else
		{
			return tdirmax.second;
		}
	};

	Node* node = select(max_more, max_less);

	while (node != nullptr)
	{
		outstring += ' ' + std::to_string(node->value);

		node = select(node->more, node->less);

		direction = !direction;
	}

	output << outstring.erase(0, 1);
}

void W1T1::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("10\n1 4 2 3 5 8 6 7 9 10\n").expect("1 4 2 8 6 7");
		reference->open(*this).input("5\n1 2 3 4 5\n").expect("1 2");
		reference->open(*this).input("1\n100\n").expect("100");
		reference->open(*this).input("12\n2 3 4 5 2 3 5 8 6 7 9 10\n").expect("2 3 2 8 6 7");
		reference->open(*this).input("15\n1 2 3 4 5 1 4 2 3 5 8 6 7 9 10\n").expect("1 2 1 4 2 8 6 7");
	}
}