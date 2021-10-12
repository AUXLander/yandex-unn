#include "w1t3.h"
#include <deque>
#include <string>
#include <stack>
#include <algorithm>

void W1T3::main(std::istream& input, std::ostream& output)
{
	std::deque<std::deque<int>> matrixA;
	std::deque<std::deque<int>> matrixB;

	std::istream_iterator<int, char> istream(input);

	const int lengthA = next(istream);
	const int lengthB = next(istream);
	const int size = next(istream);

	matrixA.resize(lengthA);
	matrixB.resize(lengthB);

	for (int index = 0; index < lengthA; ++index)
	{
		auto& column = matrixA[index];

		column.resize(size);

		for (int position = 0; position < size; ++position)
		{
			column[position] = next(istream);
		}
	}

	for (int index = 0; index < lengthB; ++index)
	{
		auto& column = matrixB[index];

		column.resize(size);

		for (int position = 0; position < size; ++position)
		{
			column[position] = next(istream);
		}
	}
	
	const auto pick = [&matrixA, &matrixB] (const int matrixIdxA,
											const int matrixIdxB,
											const int elementIdx,
											bool& side) 
	{
		side = matrixA[matrixIdxA][elementIdx] > matrixB[matrixIdxB][elementIdx];

		return side ? matrixA[matrixIdxA][elementIdx] : matrixB[matrixIdxB][elementIdx];
	};

	struct MarkUp
	{
		int left;
		int absolute;
		int right;
	};

	union
	{
		MarkUp minimum;
		int	   index_access[3];
	};

	const int lengthQ = next(istream);

	std::string result;

	for (int index = 0; index < lengthQ; ++index)
	{
		if (index > 0)
		{
			result += '\n';
		}

		const int matrixIdxA = next(istream) - 1U;
		const int matrixIdxB = next(istream) - 1U;

		bool side = true;

		int left = 0;
		int right = size - 1;

		while (std::abs(right - left) > 1U)
		{
			int middle = (left + right) / 2U;

			int value = pick(matrixIdxA, matrixIdxB, middle, side);

			if (side == false)
			{
				left = middle;
			}
			else
			{
				right = middle;
			}
		}
		
		const int zleft = pick(matrixIdxA, matrixIdxB, left, side);
		const int zright = pick(matrixIdxA, matrixIdxB, right, side);

		int zmin = pick(matrixIdxA, matrixIdxB, 0, side);
		std::string imin = "0";

		for (int i = 1; i < size; ++i)
		{
			const int z = pick(matrixIdxA, matrixIdxB, i, side);

			if (z == zmin)
			{
				imin += ", " + std::to_string(i);
			}

			if (z < zmin)
			{
				zmin = z;
				imin = std::to_string(i);
			}
		}

		if (zleft < zright)
		{
			if (imin.find(std::to_string(left)) == imin.npos)
			{
				left = left;
			}

			result += std::to_string(left + 1U);
		}
		else
		{
			if (imin.find(std::to_string(right)) == imin.npos)
			{
				right = right;
			}

			result += std::to_string(right + 1U);
		}
	}

	output << result;
}

void W1T3::test(Test* const reference)
{
	if (reference != nullptr)
	{
		// тест, который должен быть переписан, потому что используется либо первое либо последнее вхождение
		reference->open(*this)
			.input("4 3 5\n3 4 8 9 10\n2 3 15 65 66\n1 12 45 101 115\n3 3 3 3 3\n9 8 4 3 1\n99999 3456 1000 15 0\n0 0 1 0 0\n12\n1 1\n1 2\n1 3\n2 1\n2 2\n2 3\n3 1\n3 2\n3 3\n4 1\n4 2\n4 3\n")
			.expect("3\n4\n1\n2\n4\n1\n1\n4\n1\n4\n4\n3");
			//.expect("3\n5\n1\n2\n4\n1\n1\n4\n1\n4\n5\n3");

		reference->open(*this)
			.input("3 4 6\n 1 2 3 4 5 6\n1 1 1 1 1 2\n0 99999 99999 99999 99999 99999\n1 1 1 1 1 1\n5 4 3 2 1 1\n99999 123 3 3 0 0\n99999 2 1 0 0 0\n12\n1 1\n1 2\n1 3\n1 4\n2 1\n2 2\n2 3\n2 4\n3 1\n3 2\n3 3\n3 4\n")
			.expect("1\n3\n3\n2\n3\n5\n3\n3\n3\n3\n3\n3");
			//.expect("1\n3\n3\n2\n4\n5\n5\n4\n1\n1\n4\n4");
	}
}