#include <iostream>
#include <iterator>

#include <fstream>

#include <sstream>
#include <string>

#include "task.h"
#include "test.h"

//#include "task/week-1/task-1/w1t1.h"
//#include "task/week-1/task-2/w1t2.h"
//#include "task/week-1/task-3/w1t3.h"
//#include "task/week-1/task-4/w1t4.h"

//#include "task//hw-2/task-1/w2t1.hpp"
//#include "task//hw-2/task-2/w2t2.hpp"
//#include "task//hw-2/task-3/w2t3.hpp"
//#include "task/hw-2/task-4/w2t4.hpp"
//#include "task//hw-2/task-5/w2t5_correct.hpp"
//
//#include "task/hw-3/task-5/W3T5.h"
//#include "task/hw-3/task-3/W3T3.h"
//#include "task/hw-3/task-1/W3T1.hpp"
#include "task/hw-4/task-2/W4T2.hpp"
//#include "task/hw-4/task-3/W4T3.hpp"

Test test(std::cin, std::cout);

//W2T1 t1(&test);
//W2T2 t2(&test);
//W2T3 t3(&test);
//W2T4 t4(&test);
//W2T5 t5(&test);

//W3T5 t5(&test);
//
//W3T3 t3(&test);
//W3T1 t1(&test);

W4T2 t2(&test);

int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	t2.selftest();

	return 0;
}