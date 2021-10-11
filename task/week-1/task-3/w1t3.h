#pragma once
#include <iterator>
#include <iostream>
#include <string>
#include <stack>

#include "../../../task.h"
#include "../../../test.h"

/*
*
*	Longest switchable subsequence
*
*/

class W1T3 : public Task
{
	int clamp(const int min, const int value, const int max)
	{ 
		return std::min<int>(max, std::max(min, value)); 
	};

	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W1T3() : Task(nullptr) {}
	explicit W1T3(Test* const reference) : Task(reference) {}

};