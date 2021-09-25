#include <iostream>
#include <iterator>

#include <sstream>
#include <string>

#include "task.h"
#include "test.h"
#include "t1.h"
#include "t2.h"

Test test(std::cin, std::cout);

T1 t1(&test);
T2 t2(&test);

void tmain(int argc, char* argv[])
{
	t1.run(std::cin, std::cout);	
}

int main(int argc, char* argv[])
{
	t1.selftest();
	t2.selftest();

	return 0;
}