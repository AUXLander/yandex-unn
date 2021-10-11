#pragma once

#include "../../../task.h"
#include "../../../test.h"

struct  W1T2 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W1T2() : Task(nullptr) {}
	W1T2(Test* const reference) : Task(reference) {}
};