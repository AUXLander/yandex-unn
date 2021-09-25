#include <iostream>
#include <iterator>

#include <sstream>
#include <string>

#include "task.h"
#include "test.h"
#include "t1.h"

int main(int argc, char* argv[])
{
	Test test(std::cin, std::cout);
	
	T1 t1;

	test.open(t1).input("10\n1 4 2 3 5 8 6 7 9 10\n").expect("1 4 2 8 6 7");
	test.open(t1).input("5\n1 2 3 4 5\n").expect("1 2");
	test.open(t1).input("1\n100\n").expect("100");

	test.open(t1).input("12\n2 3 4 5 2 3 5 8 6 7 9 10\n").expect("2 3 2 8 6 7");

	test.open(t1).input("15\n1 2 3 4 5 1 4 2 3 5 8 6 7 9 10\n").expect("1 2 1 4 2 8 6 7");

	return 0;
}