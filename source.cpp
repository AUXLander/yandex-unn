#include <iostream>
#include <iterator>

#include <sstream>
#include <string>

#include "task.h"
#include "test.h"
#include "t1.h"

T1 t1;

void tmain(int argc, char* argv[])
{
	t1.run(std::cin, std::cout);	
}

int main(int argc, char* argv[])
{
	Test test(std::cin, std::cout);
	
	t1.selftest();

	return 0;
}