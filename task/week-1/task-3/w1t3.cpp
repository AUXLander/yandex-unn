#include "w1t3.h"
#include <vector>
#include <string>
#include <stack>
#include <algorithm>

void W1T3::main(std::istream& input, std::ostream& output)
{
	int lengthA;
	int lengthB;
	int size;
	int lengthQ;
	int matrixIdxA;
	int matrixIdxB;

	input >> lengthA;
	input >> lengthB;
	input >> size;

	int* pGlobalMatrix = new int[size * (lengthA + lengthB)];

	int* matrixA = pGlobalMatrix + 0U;
	int* matrixB = pGlobalMatrix + size * lengthA;

	for (int index = 0; index < lengthA; ++index)
	{
		for (int position = 0; position < size; ++position)
		{
			input >> matrixA[size * index + position];
		}
	}

	for (int index = 0; index < lengthB; ++index)
	{
		for (int position = 0; position < size; ++position)
		{
			input >> matrixB[size * index + position];
		}
	}

	input >> lengthQ;

	for (int index = 0; index < lengthQ; ++index)
	{
		if (index > 0)
		{
			output << '\n';
		}

		input >> matrixIdxA;
		input >> matrixIdxB;

		--matrixIdxA;
		--matrixIdxB;

		int left = 0;
		int right = size - 1;

		while (std::abs(right - left) > 1U)
		{
			int middle = (left + right) / 2U;

			if (matrixA[matrixIdxA * size + middle] <= matrixB[matrixIdxB * size + middle])
			{
				left = middle;
			}
			else
			{
				right = middle;
			}
		}

		const auto select = [&matrixA, &matrixIdxA, &matrixB, &matrixIdxB, &size](const int offset)
		{
			return std::max(matrixA[matrixIdxA * size + offset], matrixB[matrixIdxB * size + offset]);
		};

		int zmin = select(0);
		std::string imin = "0";

		for (int i = 1; i < size; ++i)
		{
			const int z = select(i);

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

		if (select(left) < select(right))
		{
			if (imin.find(std::to_string(left)) == imin.npos)
			{
				left = left;
			}

			output << left + 1U;
		}
		else
		{
			if (imin.find(std::to_string(right)) == imin.npos)
			{
				right = right;
			}

			output << right + 1U;
		}
	}

	delete[] pGlobalMatrix;
}

void W1T3::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this)
			.input("4 3 5\n3 4 8 9 10\n2 3 15 65 66\n1 12 45 101 115\n3 3 3 3 3\n9 8 4 3 1\n99999 3456 1000 15 0\n0 0 1 0 0\n12\n1 1\n1 2\n1 3\n2 1\n2 2\n2 3\n3 1\n3 2\n3 3\n4 1\n4 2\n4 3\n")
			.expect("3\n5\n1\n2\n4\n1\n1\n4\n1\n5\n5\n2");

		reference->open(*this)
			.input("3 4 6\n 1 2 3 4 5 6\n1 1 1 1 1 2\n0 99999 99999 99999 99999 99999\n1 1 1 1 1 1\n5 4 3 2 1 1\n99999 123 3 3 0 0\n99999 2 1 0 0 0\n12\n1 1\n1 2\n1 3\n1 4\n2 1\n2 2\n2 3\n2 4\n3 1\n3 2\n3 3\n3 4\n")
			.expect("1\n3\n3\n2\n5\n5\n5\n4\n1\n1\n2\n2");

		reference->open(*this)
			.input("4 3 5 1 2 3 4 5 1 1 1 1 1 0 99999 99999 99999 99999 0 0 0 0 99999 5 4 3 2 1 99999 99999 99999 0 0  99999 99999 0 0 0 12 1 1 1 2 1 3 2 1 2 2 2 3 3 1 3 2 3 3 4 1 4 2 4 3")
			.expect("3\n4\n3\n5\n4\n3\n1\n4\n3\n4\n4\n4");
	}
}