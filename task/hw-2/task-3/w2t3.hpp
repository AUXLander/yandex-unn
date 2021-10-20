#pragma once
#include <iterator>
#include <iostream>

#include "../../../task.h"
#include "../../../test.h"

/*
*
*	Longest switchable subsequence
*
*/

class W2T3 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W2T3() : Task(nullptr) {}
	explicit W2T3(Test* const reference) : Task(reference) {}
};