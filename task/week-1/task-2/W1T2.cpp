#include <stack>
#include <iterator>

#include "W1T2.h"

void W1T2::main(std::istream& input, std::ostream& output)
{
	size_t index = 0;
	std::stack<symbol<char>> stack;

	char cnext;
	bool initialized = false;

	while ((input.peek() != '\n') && (input >> cnext))
	{
		initialized = true;

		const symbol<char> snext(cnext);

		const auto codecheck = [&snext](const std::pair<symbol<char>, const rule>& cell)
		{
			return snext.code == cell.first.code;
		};

		const auto alphabet_item = std::find_if(ruleset.begin(), ruleset.end(), codecheck);

		// check is rule exist for this symbol
		if (alphabet_item != ruleset.end())
		{
			// select rule: is that symbol opening or closing
			if (alphabet_item->second == rule::close)
			{
				if (stack.empty() == false)
				{
					const auto closecheck = [&stack](const std::pair<symbol<char>, symbol<char>>& cell)
					{
						return (stack.empty() == false) && (stack.top().code == cell.first.code);
					};

					const auto closerule = std::find_if(closerules.begin(), closerules.end(), closecheck);

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

		//cnext = next(istream);
	}

	if (stack.empty() == false)
	{
		// ERROR: sequence is not closed!
		output << index << std::endl;

		return;
	}

	if (initialized == true)
	{
		output << "CORRECT" << std::endl;
	}
	else
	{
		output << 0 << std::endl;
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