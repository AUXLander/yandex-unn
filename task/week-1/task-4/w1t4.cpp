#include "w1t4.h"

#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cmath>
#include <utility>

void processPoints(const std::vector<Point>& points, std::vector<Bound>& buffer, const double radius)
{
	size_t lpos = 0;
	size_t rpos = buffer.size() - 1;

	for (const auto& point : points)
	{
		const double cxx = std::pow(radius, 2.0) - std::pow(point.coord_y, 2.0);

		if (cxx >= 0.0)
		{
			const double cx = std::pow(cxx, 0.5);

			buffer[lpos].xoffset = point.coord_x - cx;
			buffer[rpos].xoffset = point.coord_x + cx;
		}
		else
		{
			buffer[lpos].xoffset = 150000.0;
			buffer[rpos].xoffset = 150000.0;
		}

		buffer[lpos].type = true;
		buffer[rpos].type = false;

		lpos += 1;
		rpos -= 1;
	}

	std::sort(buffer.begin(), buffer.end());
}

int countPointsRadius(const std::vector<Point>& points, const std::vector<Bound>& buffer, const double radius)
{
	int max = 0;
	int timesIn = 0;
	int timesOut = 0;

	for (const auto& item : buffer)
	{
		if (item.xoffset > 2000.0)
		{
			break;
		}

		timesIn += static_cast<int>(item.type == true);
		timesOut += static_cast<int>(item.type == false);

		max = std::max(max, timesIn - timesOut);
	}

	return max;
}

void W1T4::main(std::istream& input, std::ostream& output)
{
	int size;
	int tx, ty;
	int requested_count;

	input >> size;
	input >> requested_count;

	std::vector<Point> points;
	std::vector<Bound> buffer;

	buffer.resize(2U * size);
	points.resize(1U * size);

	for (int index = 0; index < size; ++index)
	{
		input >> tx;
		input >> ty;

		points[index].coord_x = tx;
		points[index].coord_y = ty;
	}

	double start = 0.0;
	double end = 2000.0;

	while (end - start > 0.0001)
	{
		double radius = (end + start) / 2U;

		processPoints(points, buffer, radius);

		const int current_count = countPointsRadius(points, buffer, radius);

		if (current_count >= requested_count)
		{
			end = radius;
		}
		else
		{
			start = radius;
		}
	}

	output.precision(6);

	output << std::fixed << end << '\n';
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