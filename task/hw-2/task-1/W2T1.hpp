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

class W2T1 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W2T1() : Task(nullptr) {}
	explicit W2T1(Test* const reference) : Task(reference) {}
};