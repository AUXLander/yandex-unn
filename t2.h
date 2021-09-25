#pragma once
#include <iterator>
#include <iostream>
#include <string>
#include <stack>

#include "task.h"
#include "test.h"

/*
*
*	Longest switchable subsequence
*
*/

class T2 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:

	T2() : Task(nullptr) {}
	T2(Test* const reference) : Task(reference) {}

};