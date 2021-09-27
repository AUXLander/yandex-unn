#pragma once
#include <map>
#include <functional>

#include "../../../task.h"
#include "../../../test.h"
class W1T2 : public Task
{
	enum class rule
	{
		open,
		close
	};

	template<typename V>
	struct symbol
	{
		const V code;

		symbol(const V code) : code(code) {}

		struct less
		{
			bool operator()(const symbol<V>& left, const symbol<V>& right) const
			{
				return left.code < right.code;
			}
		};

		bool operator==(const symbol<V>& other) const
		{
			return this->code == other.code;
		}
	};

	const std::map<symbol<char>, const rule, symbol<char>::less> ruleset =
	{
		{ '(', rule::open }, { ')', rule::close },
		{ '[', rule::open }, { ']', rule::close },
		{ '{', rule::open }, { '}', rule::close },
	};

	const std::map<symbol<char>, symbol<char>, symbol<char>::less> closerules =
	{
		{'(', ')'},
		{'[', ']'},
		{'{', '}'},
	};

	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:	




	
	W1T2() : Task(nullptr) {}
	W1T2(Test* const reference) : Task(reference) {}

};