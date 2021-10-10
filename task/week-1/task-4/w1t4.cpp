#include "w1t4.h"

#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>

int W1T4::countPointsRadius(const std::vector<PointT>& points, const double radius)
{
	auto lspanIterator = buffer.begin();
	auto rspanIterator = buffer.rbegin();

	for (const auto& point : points)
	{
		const auto xsqr = (radius * radius) - (point.y * point.y);

		const bool isComplex = (xsqr < 0.0);

		const auto loffset = isComplex ? std::numeric_limits<double>::max() : point.x - std::sqrt(xsqr);
		const auto roffset = isComplex ? std::numeric_limits<double>::max() : point.x + std::sqrt(xsqr);

		lspanIterator->set(loffset, SpanT::RadiusDirection::left);
		rspanIterator->set(roffset, SpanT::RadiusDirection::right);

		std::advance(lspanIterator, 1U);
		std::advance(rspanIterator, 1U);
	}

	std::sort(buffer.begin(), buffer.end());

	constexpr static long int accumulation[SpanT::RadiusDirection::size] = { +1, -1 };

	int max_accumulated = 0;
	int accumulated = 0;

	for (const auto& span : buffer)
	{
		if (span.xoffset > 2001.0)
		{
			break;
		}

		accumulated += accumulation[span.direction.transformation()];

		max_accumulated = std::max(max_accumulated, accumulated);
	}

	return max_accumulated;
}

void W1T4::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this)
			.input("3 3\n0 5\n3 4\n-4 -3\n")
			.expect("5.000000", 5); // 5.000

		reference->open(*this)
			.input("3 2\n0 1\n2 1\n1 100\n")
			.expect("1.414246", 5); // 1.414
	}
}

void W1T4::main(std::istream& input, std::ostream& output)
{
	output << std::fixed;
	output.precision(6);

	std::vector<PointT> points;

	std::istream_iterator<int, char> istream(input);

	const int size = next(istream);
	const int requested_count = next(istream);

	points.reserve(size);

	for (int index = 0; index < size; index++)
	{
		const auto x = next(istream);
		const auto y = next(istream);

		points.emplace_back(x, y);
	}

	initialize(points.size());

	double start = 0.0;
	double end = 1415.0;
	double radius;

	const auto check = [this, &points, &requested_count] (const double radius) -> bool
	{
		return countPointsRadius(points, radius) >= requested_count;
	};

	while ((end - start) > precision)
	{
		radius = (end + start) * 0.5;

		if (check(radius) == true)
		{
			end = radius;
		}
		else
		{
			start = radius;
		}
	}

	output << end << std::endl;
}