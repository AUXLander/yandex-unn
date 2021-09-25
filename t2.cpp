#include "t2.h"
#include <deque>

void T2::main(std::istream& input, std::ostream& output)
{
	std::deque<std::deque<int>> matrixA;
	std::deque<std::deque<int>> matrixB;

	std::istream_iterator<int, char> istream(input);

	const int lengthA = next(istream);
	const int lengthB = next(istream);
	const size_t size = next(istream);

	for (size_t index = 0; index < lengthA; ++index)
	{
		auto& column = matrixA.emplace_back();

		for (size_t position = 0; position < size; ++position)
		{
			column.push_back(next(istream));
		}
	}

	for (size_t index = 0; index < lengthB; ++index)
	{
		auto& column = matrixB.emplace_back();

		for (size_t position = 0; position < size; ++position)
		{
			column.push_back(next(istream));
		}
	}
	
	std::string outstring;
	const int lengthQ = next(istream);

	for (size_t index = 0; index < lengthQ; ++index)
	{
		const int kA = next(istream) - 1;
		const int kB = next(istream) - 1;

		int k = 0;
		int min = std::max(matrixA[kA][k], matrixB[kB][k]);

		for (int position = 0; position < size; ++position)
		{
			const int locmin = std::max(matrixA[kA][position], matrixB[kB][position]);

			if (locmin < min)
			{
				min = locmin;
				k = position;
			}
		}

		outstring += '\n';
		outstring += std::to_string(k + 1);
	}

	output << outstring.erase(0, 1);
}

void T2::test(Test* const reference)
{
	if (reference != nullptr)
	{
		// тест, который должен быть переписан, потому что используется либо первое либо последнее вхождение
		reference->open(*this)
			.input("4 3 5\n1 2 3 4 5\n1 1 1 1 1\n0 99999 99999 99999 99999\n0 0 0 0 99999\n5 4 3 2 1\n99999 99999 99999 0 0\n99999 99999 0 0 0\n12\n1 1\n1 2\n1 3\n2 1\n2 2\n2 3\n3 1\n3 2\n3 3\n4 1\n4 2\n4 3\n")
			.expect("3\n4\n3\n5\n4\n3\n1\n4\n3\n4\n4\n4\n");
	}
}