#include <vector>
#include <list>
#include <array>
#include <stack>
#include <cmath>
#include <deque>

#include "../../../test.h"
#include "w1t1.h"

void W1T1::main(std::istream& input, std::ostream& output)
{
	struct Element
	{
		int value;

		std::pair<size_t, Element*> more;
		std::pair<size_t, Element*> less;

		Element(std::stack<int>& stack, std::pair<size_t, Element*>& more, std::pair<size_t, Element*>& less)
			: value(stack.top()), more(more), less(less) {}
	};

	int length;
	std::string result;
	std::stack<int> stack;
	std::deque<Element> nodes;

	std::pair<size_t, Element*> more(0, nullptr);
	std::pair<size_t, Element*> less(0, nullptr);

	bool dir = (more.first < less.first);

	const auto select = [&dir] (const std::pair<size_t, Element*>& fdirmax, const std::pair<size_t, Element*>& tdirmax)
	{
		if (dir == false)
		{
			return fdirmax.second;
		}
		else
		{
			return tdirmax.second;
		}
	};

	input >> length;

	for (int i = 0; i < length; ++i)
	{
		int value;

		input >> value;

		stack.push(value);
	}

	while (!stack.empty())
	{
		std::pair<size_t, Element*> tmore(0, nullptr);
		std::pair<size_t, Element*> tless(0, nullptr);

		for (size_t index = 0; index < nodes.size(); index++)
		{
			Element& node = nodes[nodes.size() - 1 - index];

			if (node.value > stack.top())
			{
				if (node.less.first + 1 > tmore.first)
				{
					tmore.first = node.less.first + 1;
					tmore.second = &node;
				}
			}

			if (node.value < stack.top())
			{
				if (node.more.first + 1 > tless.first)
				{
					tless.first = node.more.first + 1;
					tless.second = &node;
				}
			}
		}

		nodes.push_back({ stack, tmore, tless });

		if (nodes.back().more.first >= more.first)
		{
			more.first = nodes.back().more.first;
			more.second = &nodes.back();
		}

		if (nodes.back().less.first >= less.first)
		{
			less.first = nodes.back().less.first;
			less.second = &nodes.back();
		}

		stack.pop();
	}

	Element* pointer = select(more, less);

	while (pointer != nullptr)
	{
		result += ' ' + std::to_string(pointer->value);

		pointer = select(pointer->more, pointer->less);

		dir = !dir;
	}

	if (length > 0)
	{
		output << result.substr(1, -1);
	}
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