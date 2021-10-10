#include "w1t4.h"

#include <map>

#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iomanip>

#include <random>

template<class T>
struct Point
{
	using value_type = T;

	value_type x;
	value_type y;

	inline bool operator<(const Point& other) const
	{
		return (this->x < other.x) || ((this->x == other.x) && (this->y < other.y));
	}
};

template<class T>
struct Span
{
	using value_type = T;

	struct RadiusDirection
	{
		using value_type = size_t;
		enum Direction : value_type
		{
			left = 0, right = 1
		};

		// The power of Direction set;
		constexpr static size_t power { 2U };

	private:
		Direction direction;
	public:

		RadiusDirection(const RadiusDirection::Direction direction) : direction(direction) {}

		inline value_type transformation() const
		{
			return static_cast<value_type>(this->direction);
		}
	};

public:
	value_type xoffset;
	RadiusDirection direction;

	Span() = delete;
	Span(const value_type xoffset, const RadiusDirection direction) : xoffset(xoffset), direction(direction) {}

	inline bool operator<(const Span<T>& other) const
	{
		return this->xoffset < other.xoffset;
	}
};

using SpanT = Span<double>;
using PointT = Point<double>;

void calculateSpans(const std::vector<PointT>& points, std::multiset<SpanT>& spans, const double radius)
{
	for (const auto& point : points) // 2 * n * log(n)
	{
		const auto xsqr = (radius * radius) - (point.y * point.y);

		const bool isComplex = (xsqr < 0.0);

		const auto x = isComplex ? 0.0 : std::sqrt(xsqr);

		const auto loffset = isComplex ? std::numeric_limits<double>::max() : point.x - x;
		const auto roffset = isComplex ? std::numeric_limits<double>::max() : point.x + x;

		const auto& lspan = spans.emplace(loffset, SpanT::RadiusDirection::left); // log(n)
		const auto& rspan = spans.emplace(roffset, SpanT::RadiusDirection::right); // log(n)
	}
}

size_t calculateK(const std::multiset<SpanT>& spans)
{
	constexpr static size_t positive_accumulation[SpanT::RadiusDirection::power] = { 1U, 0U };
	constexpr static size_t negative_accumulation[SpanT::RadiusDirection::power] = { 0U, 1U	};
	
	size_t max_accumulated = 0;
	size_t accumulated = 0;

	for (const auto& span : spans)
	{
		if (span.xoffset > 2001.0)
		{
			break;
		}

		accumulated += positive_accumulation[span.direction.transformation()];
		accumulated -= negative_accumulation[span.direction.transformation()];

		max_accumulated = std::max(max_accumulated, accumulated);
	}

	return max_accumulated;
}

void W1T4::test(Test* const reference)
{
	constexpr double precision = 1e-4;
	const std::vector<PointT> points = {
		{ 0.0,  5.0},
		{ 3.0,  4.0},
		{-4.0, -3.0}
	};

	std::multiset<SpanT> spans;

	double start = 0.0;
	double end = 1415.0;
	double radius;

	const size_t k = 3U;

	while ((end - start) > precision)
	{
		radius = (end + start) * 0.5;

		calculateSpans(points, spans, radius);

		if (calculateK(spans) >= k)
		{
			end = radius;
		}
		else
		{
			start = radius;
		}

		spans.clear();
	}

	std::cout << std::fixed;
	std::cout.precision(6);

	std::cout << "R: " << end << std::endl;
}

void W1T4::main(std::istream& input, std::ostream& output)
{

}