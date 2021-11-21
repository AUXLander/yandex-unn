#pragma once
#include <iterator>
#include <iostream>

#include "../../../task.h"
#include "../../../test.h"

class W3T3 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W3T3() : Task(nullptr) {}
	explicit W3T3(Test* const reference) : Task(reference) {}
};

