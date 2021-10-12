#include "w1t3.h"
#include <vector>
#include <string>
#include <stack>
#include <algorithm>

void W1T3::main(std::istream& input, std::ostream& output)
{
	int length[3];

	int size;
	int index_a;
	int index_b;

	input >> length[0];
	input >> length[1];
	input >> size;

	int* pGlobalMatrix = new int[size * (length[0] + length[1])];

	int* arrays_a = pGlobalMatrix + 0U;
	int* arrays_b = pGlobalMatrix + size * length[0];

	for (int idx = 0; idx < length[0] + length[1]; ++idx)
	{
		for (int ipx = 0; ipx < length[0] + length[1]; ++ipx)
		{
			for (int position = 0; position < size; ++position)
			{
				for (int index = 0; index < size; ++index)
				{
					pGlobalMatrix[idx * size + index] = 
						std::max(
							std::max(
								std::max(position, index),
							idx),
						ipx);
				}
			}
		}
	}

	for (int index = 0; index < length[0]; ++index)
	{
		for (int position = 0; position < size; ++position)
		{
			input >> arrays_a[size * index + position];
		}
	}

	for (int index = 0; index < length[1]; ++index)
	{
		for (int position = 0; position < size; ++position)
		{
			input >> arrays_b[size * index + position];
		}
	}

	input >> length[2];

	for (int index = 0; index < length[2]; ++index)
	{
		if (index > 0)
		{
			output << '\n';
		}

		input >> index_a;
		input >> index_b;

		--index_a;
		--index_b;

		int top = 0;
		int bottom = size - 1;

		while (std::abs(bottom - top) > 1U)
		{
			int mid = (top + bottom) / 2U;

			if (arrays_a[index_a * size + mid] <= arrays_b[index_b * size + mid])
			{
				top = mid;
			}
			else
			{
				bottom = mid;
			}
		}

		const auto select = [&arrays_a, &index_a, &arrays_b, &index_b, &size](const int offset)
		{
			return std::max(arrays_a[index_a * size + offset], arrays_b[index_b * size + offset]);
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

		if (select(top) < select(bottom))
		{
			if (imin.find(std::to_string(top)) == imin.npos)
			{
				top = top;
			}

			output << top + 1U;
		}
		else
		{
			if (imin.find(std::to_string(bottom)) == imin.npos)
			{
				bottom = bottom;
			}

			output << bottom + 1U;
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