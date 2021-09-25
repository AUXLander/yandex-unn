#include <vector>
#include <array>
#include <string>
#include <stack>
#include <iterator>
#include <cmath>
#include <iostream>

#include "task.h"

struct T1 : public Task
{
	const std::istream_iterator<int> EOFR;

	T1() : Task() {}

	struct Node
	{
		int value;
		size_t count_more;
		Node* more_ptr;

		size_t count_less;
		Node* less_ptr;

		Node(int v, size_t cm, Node* pmore, size_t cl, Node* pless)
			: value(v), count_more(cm), more_ptr(pmore), count_less(cl), less_ptr(pless)
		{

		}
	};

	std::string subrun(const int length, std::stack<int> stack)
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
					if (it->count_less + 1 > max_count_more)
					{
						max_count_more = it->count_less + 1;
						max_more_ptr = &(*it);
					}
				}
				
				if (it->value <= top)
				{
					if (it->count_more + 1 > max_count_less)
					{
						max_count_less = it->count_more + 1;
						max_less_ptr = &(*it);
					}
				}
			}

			nodes.emplace_back(top, max_count_more, max_more_ptr, max_count_less, max_less_ptr);

			stack.pop();
		}

		int index_count_more = nodes.size() - 1;
		size_t max_count_more = nodes[index_count_more].count_more;

		int index_count_less = nodes.size() - 1;
		size_t max_count_less = nodes[index_count_less].count_less;

		for (int index = nodes.size() - 1; index >= 0; --index)
		{
			const auto& node = nodes[index];

			if (node.count_more > max_count_more)
			{
				max_count_more = node.count_more;
				index_count_more = index;
			}

			if (node.count_less > max_count_less)
			{
				max_count_less = node.count_less;
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

		Node* next_ptr = direction ? node.less_ptr : node.more_ptr;

		direction = !direction;

		while (next_ptr != nullptr)
		{
			node = *next_ptr;

			output += ' ' + std::to_string(node.value);

			next_ptr = direction ? node.less_ptr : node.more_ptr;

			direction = !direction;
		}

		return output;
	}

	void run(std::istream& input, std::ostream& output) override final
	{
		Task::reset();

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
};