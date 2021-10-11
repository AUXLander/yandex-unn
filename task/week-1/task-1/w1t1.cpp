#include <vector>
#include <list>
#include <array>
#include <stack>
#include <cmath>

#include "../../../test.h"
#include "w1t1.h"

struct Node;

typedef std::pair<size_t, Node*> NodeCounter;

struct Node
{
	int value;

	NodeCounter more;
	NodeCounter less;

	Node(int v, size_t cm, Node * const pmore, size_t cl, Node* const pless)
		: value(v), more(cm, pmore), less(cl, pless)
	{
	}
};

void W1T1::main(std::istream& input, std::ostream& output)
{
	std::list<Node> nodes;
	std::stack<int> stack;

	int length;
	
	input >> length;

	for (int i = 0; i < length; ++i)
	{
		int value;

		input >> value;

		stack.push(value);
	}

	std::string outstring;

	NodeCounter max_more(0, nullptr);
	NodeCounter max_less(0, nullptr);

	while (stack.empty() == false)
	{
		const auto top = stack.top();

		NodeCounter loc_max_more(0, nullptr);
		NodeCounter loc_max_less(0, nullptr);

		for (auto it = nodes.rbegin(); it != nodes.rend(); ++it)
		{
			if ((it->value >= top) && (it->less.first + 1 > loc_max_more.first))
			{
				loc_max_more.first = it->less.first + 1;
				loc_max_more.second = &(*it);
			}

			if ((it->value <= top) && (it->more.first + 1 > loc_max_less.first))
			{
				loc_max_less.first = it->more.first + 1;
				loc_max_less.second = &(*it);
			}
		}

		auto& node = nodes.emplace_back(top, loc_max_more.first, loc_max_more.second, loc_max_less.first, loc_max_less.second);

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

	Node* node = !direction ? max_more.second : max_less.second;

	while (node != nullptr)
	{
		outstring += ' ' + std::to_string(node->value);

		node = direction ? node->less.second : node->more.second;

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