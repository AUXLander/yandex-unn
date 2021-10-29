#pragma once
#include <iterator>
#include <iostream>
#include <deque>
#include <random>

#include "../../../task.h"
#include "../../../test.h"

class W2T4 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W2T4() : Task(nullptr) {}
	explicit W2T4(Test* const reference) : Task(reference) {}
};