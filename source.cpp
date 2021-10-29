#include <iostream>
#include <iterator>

#include <fstream>

#include <sstream>
#include <string>

#include "task.h"
#include "test.h"

#include "task/week-1/task-1/w1t1.h"
#include "task/week-1/task-2/w1t2.h"
#include "task/week-1/task-3/w1t3.h"
#include "task/week-1/task-4/w1t4.h"

//#include "task//hw-2/task-1/w2t1.hpp"
//#include "task//hw-2/task-2/w2t2.hpp"
//#include "task//hw-2/task-3/w2t3.hpp"
//#include "task/hw-2/task-4/w2t4.hpp"
//#include "task//hw-2/task-5/w2t5_correct.hpp"

#include "task/hw-3/task-5/W3T5.h"

Test test(std::cin, std::cout);

//W2T1 t1(&test);
//W2T2 t2(&test);
//W2T3 t3(&test);
//W2T4 t4(&test);
//W2T5 t5(&test);

W3T5 t5(&test);


int main(int argc, char* argv[])
{
	//std::ios_base::sync_with_stdio(false);
	//std::cin.tie(nullptr);

	//t1.selftest();

	//t2.selftest();

	//std::ifstream file_in("E:\\UserData\\Projects\\Yandex\\yandex-unn\\input.txt");
	//std::ifstream file_in("input.txt");

//	t3.run(std::cin, std::cout);

	//t3.selftest();

	//t5.run(file_in, std::cout);
	
	t5.selftest();

	//t5.selftest();


	return 0;
}