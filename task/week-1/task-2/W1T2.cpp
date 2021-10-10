#include <stack>
#include <iterator>

#include "W1T2.h"

void W1T2::main(std::istream& input, std::ostream& output)
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

void W1T2::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("(())\n").expect("CORRECT\n");
		reference->open(*this).input("([)]\n").expect("2\n");
		reference->open(*this).input("(([{\n").expect("4\n");

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