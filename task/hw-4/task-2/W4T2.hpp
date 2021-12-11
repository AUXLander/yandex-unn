#pragma once
#include <iterator>
#include <iostream>

#include "../../../task.h"
#include "../../../test.h"

class W4T2 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W4T2() : Task(nullptr) {}
	explicit W4T2(Test* const reference) : Task(reference) {}
};

