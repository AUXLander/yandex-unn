#include "w1t3.h"
#include <deque>

void W1T3::main(std::istream& input, std::ostream& output)
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
	
	const auto element = [&matrixA, &matrixB](const int i, const int j, const int k) { return std::max(matrixA[i][k], matrixB[j][k]); };

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

	std::string outstring;

	const int mid = size / 2U;
	const int limit_offset = (size / 2U) + 1;

	const int lengthQ = next(istream);

	for (size_t index = 0; index < lengthQ; ++index)
	{
		const int i = next(istream) - 1U;
		const int j = next(istream) - 1U;

		int k = mid;

		minimum.absolute = element(i, j, k);

		minimum.left = minimum.absolute;
		minimum.right = minimum.absolute;

		for (int offset = 1; offset < limit_offset; ++offset)
		{
			for (int sign = -1; sign < 2; sign += 2)
			{
				const int position = clamp(0, mid - sign * offset, size - 1U);
				const int locmin = element(i, j, position);

				index_access[1U + sign] = locmin;

				if (locmin < minimum.absolute)
				{
					minimum.absolute = locmin;
					k = position;
				}
			}

			if ((minimum.left > minimum.absolute) && (minimum.absolute < minimum.right))
			{
				break;
			}
		}

		outstring += '\n';
		outstring += std::to_string(k + 1U);
	}

	output << outstring.erase(0, 1);
}

void W1T3::test(Test* const reference)
{
	if (reference != nullptr)
	{
		// ����, ������� ������ ���� ���������, ������ ��� ������������ ���� ������ ���� ��������� ���������
		reference->open(*this)
			.input("4 3 5\n3 4 8 9 10\n2 3 15 65 66\n1 12 45 101 115\n3 3 3 3 3\n9 8 4 3 1\n99999 3456 1000 15 0\n0 0 1 0 0\n12\n1 1\n1 2\n1 3\n2 1\n2 2\n2 3\n3 1\n3 2\n3 3\n4 1\n4 2\n4 3\n")
			.expect("3\n5\n1\n2\n4\n1\n1\n4\n1\n4\n5\n3");

		reference->open(*this)
			.input("3 4 6\n 1 2 3 4 5 6\n1 1 1 1 1 2\n0 99999 99999 99999 99999 99999\n1 1 1 1 1 1\n5 4 3 2 1 1\n99999 123 3 3 0 0\n99999 2 1 0 0 0\n12\n1 1\n1 2\n1 3\n1 4\n2 1\n2 2\n2 3\n2 4\n3 1\n3 2\n3 3\n3 4\n")
			.expect("1\n3\n3\n2\n4\n5\n5\n4\n1\n1\n4\n4");
	}
}