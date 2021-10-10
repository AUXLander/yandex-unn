#pragma once
#include <iterator>
#include <iostream>
#include <vector>


#include "../../../task.h"
#include "../../../test.h"


struct SimpleSimplex
{
	using Row = std::vector<double>;

	//struct Row : public std::vector<double>
	//{
	//	using value_type = double;

	//	//value_type x1;
	//	//value_type x2;
	//	//value_type y1;
	//	//value_type y2;
	//	//value_type Bi;
	//	//value_type del;

	//	//inline value_type& at(size_t index);
	//	//inline const value_type& at(size_t index) const;
	//};

	struct Answer
	{
		Row::value_type x1;
		Row::value_type x2;

		Row::value_type optimum;
	};


	//constexpr static size_t row_length = sizeof(Row) / sizeof(Row::value_type);

	bool isReferenceValid(const Row& reference, size_t& min_invalid_index) const
	{
		bool result = true;
		min_invalid_index = 0;

		// without 2 last fields
		for (size_t index = 0; index < reference.size() - 2; index++)
		{
			if (reference.at(index) < 0.0)
			{
				result = false;

				if ((reference.at(index) < reference.at(min_invalid_index))
					|| (reference.at(min_invalid_index) <= 0))
				{
					min_invalid_index = index;
				}
			}
		}

		return result;
	}

	Answer next(const std::vector<Row>& restrictions)
	{
		const size_t size = restrictions.size();
		const size_t last_index = size - 1;

		std::vector<Row> table(restrictions);
		std::vector<Row> buffer(restrictions);
		std::vector<Row::value_type> coefficients(size, 0);

		size_t min_column_index = 0;

		while (isReferenceValid(table[last_index], min_column_index) == false)
		{
			size_t min_row_index = 0;

			// делим Bi из каждой строки на значени€ из ведущего столбца x1 или x2
			// кроме последней потому что там целева€ функци€
			for (size_t rowIdx = 0; rowIdx < size - 1; ++rowIdx)
			{
				auto& row = table[rowIdx];
				auto& del = row[row.size() - 1];
				const auto& Bi  = row[row.size() - 2];

				//row.del = row.Bi / row.at(min_column_index);

				const auto z = row.at(min_column_index);

				if (z > 1e-5)
				{
					del = Bi / z;
				}
				else
				{
					del = -1;
				}
				
				if ((del > 0) && (del < table[min_row_index][row.size() - 1]))
				{
					min_row_index = rowIdx;
				}
			}

			const Row::value_type pivot = table[min_row_index].at(min_column_index);

			// делим все €чейки ведущего столбца на ведущий элемент, чтобы потом домножить строчку на нужный коэффициент, вычесть и получить 0
			for (size_t rowIdx = 0; rowIdx < size; ++rowIdx)
			{
				coefficients[rowIdx] = table[rowIdx].at(min_column_index) / pivot;
			}

			// соответствующей самой себе будет = 0
			coefficients[min_row_index] = 0;

			for (size_t rowIdx = 0; rowIdx < size; ++rowIdx)
			{
				for (size_t columnIdx = 0; columnIdx < table[rowIdx].size(); ++columnIdx)
				{
					buffer[rowIdx].at(columnIdx) = table[rowIdx].at(columnIdx) - coefficients[rowIdx] * table[min_row_index].at(columnIdx);
				}
			}

			std::swap(table, buffer);
		}


		size_t index = 0;
		Row::value_type data[2] {0, 0};

		for (size_t columnIdx = 0; (columnIdx < table[0].size() - 2) && (index < 2); ++columnIdx)
		{
			size_t count_not_zeros = 0;

			for (size_t rowIdx = 0; rowIdx < size - 1; ++rowIdx)
			{
				if (std::abs(table[rowIdx].at(columnIdx)) > 1e-5)
				{
					const auto& Bi = table[rowIdx][table[rowIdx].size() - 2];

					data[index] = Bi / table[rowIdx].at(columnIdx);
					++count_not_zeros;
				}
			}

			if (count_not_zeros == 1)
			{
				index++;
			}
		}

		return { data[0] , data[1], table[last_index][table[last_index].size() - 2] };
	}
};


class W1T4 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:

	W1T4() : Task(nullptr) {}
	W1T4(Test* const reference) : Task(reference) {}

};