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

#include "task//hw-2/task-2/w2t2.hpp"
#include "task//hw-2/task-5/w2t5.hpp"

Test test(std::cin, std::cout);

W2T2 t2(&test);
W2T5 t5(&test);

void tmain(int argc, char* argv[])
{
	
}

int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	//t2.selftest();

	//std::ifstream file_in("E:\\UserData\\Projects\\Yandex\\yandex-unn\\input.txt");
	//std::ifstream file_in("input.txt");

	//t5.run(file_in, std::cout);
	t5.selftest();

	return 0;
}