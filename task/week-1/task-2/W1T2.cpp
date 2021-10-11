#include <stack>
#include <iterator>

#include "W1T2.h"

static bool selectRule(const char ch)
{
	switch (ch)
	{
	case '(': return true;
	case ')': return false;

	case '{': return true;
	case '}': return false;

	case '[': return true;
	case ']': return false;
	}

	return false;
}

static char selectCloseRule(const char ch)
{
	switch (ch)
	{
	case '(': return ')';
	case '{': return '}';
	case '[': return ']';
	}
	return '\0';
}

int parse(std::stack<char>& stack, size_t& index, const char cnext)
{
	const bool isRuleExist = (cnext == '(') || (cnext == '[') || (cnext == '{' || (cnext == '}') || (cnext == ']') || (cnext == ')'));

	if (isRuleExist == false)
	{
		return index;
	}

	if (selectRule(cnext) == true) // rule::open
	{
		stack.push(cnext);
		index++;

		return -1;
	}

	if (stack.empty() == true)
	{
		return index;
	}

	if (selectCloseRule(stack.top()) != cnext)
	{
		return index;
	}

	stack.pop();
	index++;

	return -1;
}

void W1T2::main(std::istream& input, std::ostream& output)
{
	size_t index = 0;
	std::stack<char> stack;

	char cnext;
	int status = -2;
	bool initialized = false;

	while ((input.peek() != '\n') && (input >> cnext) && (status < 0))
	{
		status = parse(stack, index, cnext);
	}

	if (status < 0)
	{
		if (stack.empty())
		{
			if (status == -1)
			{
				output << "CORRECT" << '\n';
			}
			else
			{
				output << 0 << '\n';
			}
		}
		else
		{
			output << index << '\n';
		}
	}
	else
	{
		output << index << '\n';
	}
}

void W1T2::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("(())\n").expect("CORRECT\n");
		reference->open(*this).input("([)]\n").expect("2\n");
		reference->open(*this).input("(([{\n").expect("4\n");
		reference->open(*this).input("\n").expect("0\n");

		reference->open(*this).input("(())(()(\n").expect("8\n");
		reference->open(*this).input("[(()[]]\n").expect("6\n");
		reference->open(*this).input("{([(())]}\n").expect("8\n");
		reference->open(*this).input(")[]{}\n").expect("0\n");
		reference->open(*this).input("(])\n").expect("1\n");
		reference->open(*this).input("{(]))\n").expect("2\n");
		reference->open(*this).input("[](){}())\n").expect("8\n");
		reference->open(*this).input("[(()){}](){}\n").expect("CORRECT\n");
	}
}