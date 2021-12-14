#include <vector>
#include <list>
#include <array>
#include <stack>
#include <cmath>

#include "../../../test.h"
#include "w1t1.h"

struct Node
{
	int p_root;

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

	Node(int v, size_t cm, Node * const pmore, size_t cl, Node* const pless)
		: p_root(v), more(cm, pmore), less(cl, pless)
	{
	}
};

void W1T1::main(std::istream& input, std::ostream& output)
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
				if ((it->p_root >= top) && (it->less.count + 1 > loc_max_more.count))
				{
					loc_max_more.copy(it->less.count + 1, &(*it));
				}

				if ((it->p_root <= top) && (it->more.count + 1 > loc_max_less.count))
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
			outstring += ' ' + std::to_string(node->p_root);

			node = direction ? node->less.pointer : node->more.pointer;

			direction = !direction;
		}

		output << outstring.erase(0, 1);
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