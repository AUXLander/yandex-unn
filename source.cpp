#include <iostream>
#include <iterator>

#include <sstream>
#include <string>

#include "task.h"
#include "test.h"

#include "task/week-1/task-1/w1t1.h"
#include "task/week-1/task-2/w1t2.h"
#include "task/week-1/task-3/w1t3.h"
#include "task/week-1/task-4/w1t4.h"

Test test(std::cin, std::cout);

W1T1 t1(&test);
W1T2 t2(&test);
W1T3 t3(&test);
W1T4 t4(&test);

void tmain(int argc, char* argv[])
{
	t1.run(std::cin, std::cout);	
}

int main(int argc, char* argv[])
{
	t1.selftest();
	/*t2.selftest();
	t3.selftest();
	t4.selftest();*/

	return 0;
}