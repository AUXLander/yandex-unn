#pragma once
#include <iterator>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "../../../task.h"
#include "../../../test.h"

class W3T5 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W3T5() : Task(nullptr) {}
	explicit W3T5(Test* const reference) : Task(reference) {}
};
