#include "w1t4.h"

#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iomanip>
#include <random>

//SimpleSimplex::Row::value_type& SimpleSimplex::Row::at(const size_t index)
//{table[last_index]
//	return reinterpret_cast<SimpleSimplex::Row::value_type*>(this)[index];
//}
//
//const SimpleSimplex::Row::value_type& SimpleSimplex::Row::at(const size_t index) const
//{
//	return reinterpret_cast<const SimpleSimplex::Row::value_type*>(this)[index];
//}

inline int countSetBits(const uint64_t n)
{
	return (n == 0U) ? 0U : 1U + countSetBits(n & (n - 1U));
}


size_t operations = 0;

struct Point
{
	double x;
	double y;

	std::multiset<double> links;

	Point(const double x, const double y) : x(x), y(y) {}

	static double length(const Point& p1, const Point& p2)
	{
		const auto xx = (p2.x - p1.x) * (p2.x - p1.x);
		const auto yy = (p2.y - p1.y) * (p2.y - p1.y);

		return std::sqrt(xx + yy);
	};

	inline double length(const Point& other) const
	{
		return Point::length(*this, other);
	}

	inline bool operator==(const Point& other) const
	{
		return (x == other.x) && (y == other.y);
	}
};


int gray_code(int n) {
	return n ^ (n >> 1);
}

int count_bits(int n) {
	int res = 0;
	for (; n; n >>= 1)
		res += n & 1;
	return res;
}

double getMinRadius(const std::vector<Point>& source)
{
	struct
	{
		double min = std::numeric_limits<double>::max();
		double max = std::numeric_limits<double>::min();;
	} xoffset;

	for (const auto& gamma : source)
	{
		for (const auto& element : source)
		{
			if ((gamma == element) == false)
			{
				const auto xg = gamma.x;
				const auto xi = element.x;

				const auto xxg = std::pow(gamma.x, 2.0);
				const auto yyg = std::pow(gamma.y, 2.0);

				const auto xxi = std::pow(element.x, 2.0);
				const auto yyi = std::pow(element.y, 2.0);

				const auto nom = (xxg + yyg) - (xxi + yyi);
				const auto dnm = 2.0 * (xg - xi);

				if (dnm != 0)
				{
					const auto v = nom / dnm;

					xoffset.max = std::max(xoffset.max, v);
					xoffset.min = std::min(xoffset.min, v);

					//std::cout << std::setw(2) << "v = " << v << "  g: (" << gamma.x << " " << gamma.y << ") e: (" << element.x << " " << element.y << ")" << std::endl;
				}
				else
				{
					std::cout << "div zero" << std::endl;
				}

				operations++;
			}
		}
	}

	const auto lambda = [](const std::vector<Point>& buffer, const double offset) -> double
	{
		double rmax = std::numeric_limits<double>::min();

		for (const auto& element : buffer)
		{
			const auto x = element.x;
			const auto y = element.y;

			rmax = std::max(rmax, std::pow(x - offset, 2.0) + std::pow(y, 2.0));

			operations++;
		}

		return rmax;
	};

	const auto f = [&source, &lambda](const double offset) { return lambda(source, offset); };

	const size_t size = 1000U;
	const double dx = (xoffset.max - xoffset.min) / static_cast<double>(size);

	const auto unimodSearch = [&f](double start, double end, const double dx)
	{
		const double phi = 2.0 / (1.0 + 2.2360679774997896964091736687313);

		double x = 0;

		while (dx > 1e-10)
		{
			const double length = end - start;

			const double x1 = end - length * phi;
			const double x2 = start + length * phi;

			if (f(x1) >= f(x2))
			{
				start = x1;
			}
			else
			{
				end = x2;
			}

			if (std::abs(end - start) < dx)
			{
				return (start + end) / 2.0;
			}

			operations++;
		}

		return end;
	};

	const double offset = unimodSearch(xoffset.min, xoffset.max, dx);

	return f(offset);
}

inline uint64_t bitMaskOfLen(const uint8_t len)
{
	return (len < 63) ? ((uint64_t(1) << len) - 1) : -1;
}


void all_combinations(int n, int k) {
	for (int i = 0; i < (1 << n); ++i) {
		int cur = gray_code(i);
		if (count_bits(cur) == k) {
			for (int j = 0; j < n; ++j)
			{
				if (cur & (1 << j))
				{
					printf("%d ", j + 1);
				}
			}
			//puts("");
		}
	}
}

void W1T4::test(Test* const reference)
{
	//const std::vector<Point> list = {
	//	{-4, -3},
	//	{ 0,  5},
	//	{ 2,  5},
	//	{ 3,  4},
	//};
	//const size_t k = 3;

	std::vector<Point> list;

	for (size_t j = 0; j < 64; j++)
	{
		const double x = std::round(-1000.0 + 2000.0 * rand() / static_cast<double>(RAND_MAX));
		const double y = std::round(-1000.0 + 2000.0 * rand() / static_cast<double>(RAND_MAX));

		list.emplace_back(x, y);
	}

	//const std::vector<Point> list = {
	//	{ 0, 1 },
	//	{ 2, 1 },
	//	{ 1, 100 },
	//	/*{-4, -3},
	//	{ 0,  5},
	//	{ 2,  5},
	//	{ 3,  4},
	//	{ 0, 111},
	//	{ 100, 103},
	//	{ 102, 103}*/
	//};
	const size_t k = 2;

	// solution 1

	//std::vector<Point> xprojections;

	//for (const auto& point : list)
	//{
	//	auto& xpr = xprojections.emplace_back(point.x, 0);

	//	for (auto& point : list)
	//	{
	//		xpr.links.emplace();

	//		if (xpr.links.size() > k)
	//		{
	//			xpr.links.erase(--xpr.links.end());
	//		}
	//	}
	//}

	//double min = *(--xprojections.front().links.end());

	//for (auto& xpr : xprojections)
	//{
	//	min = std::min(min, *(--xpr.links.end()));
	//}

	//// solution 2

	//std::multiset<double> bucket;

	//double min1 = std::numeric_limits<double>::max();

	//for (const auto& point : list)
	//{
	//	auto& xpr = xprojections.emplace_back(point.x, 0);

	//	for (auto& point : list)
	//	{
	//		bucket.emplace(xpr.length(point));

	//		if (bucket.size() > k + 1)
	//		{
	//			bucket.erase(--bucket.end());
	//		}
	//	}

	//	min1 = std::min(*--bucket.end(), min1);

	//	bucket.clear();
	//}

	// solution 3

	//const std::vector<Point> rlist = {
	//	{-4, -3},
	//	{ 0,  5},
	//	{ 2,  5},
	//	{ 3,  4}
	//};

	std::vector<Point> generation;

	size_t unical = 0;

	double minr = std::numeric_limits<double>::max();

	const auto& source = list;
	
	const size_t n = source.size();

	for (int i = 0; i < (1 << n); ++i) {
		int cur = gray_code(i);
		if (count_bits(cur) == k) {
			for (int j = 0; j < n; ++j)
			{
				if (cur & (1 << j))
				{
					printf("%d ", j + 1);
				}
			}
					
			printf("\n");
		}
	}

	for (int i = 0; i < (1 << n); ++i)
	{
		int cur = gray_code(i);

		if (count_bits(cur) == k) 
		{
			for (int j = 0; j < n; ++j)
			{
				if (cur & (1 << j))
				{
					generation.push_back(source[j + 1]);
				}
			}

			const double RR = getMinRadius(generation);

			minr = std::min(minr, std::sqrt(RR));

			unical++;

			generation.clear();
		}
	}

	std::cout << "R: " << minr << std::endl;
	std::cout << "unical: " << unical << std::endl;
	std::cout << "operations: " << operations << std::endl;

 	return;
}

void W1T4::main(std::istream& input, std::ostream& output)
{

}