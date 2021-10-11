#pragma once
#include <iterator>
#include <iostream>
#include <vector>


#include "../../../task.h"
#include "../../../test.h"

template<class T>
struct Point
{
	using value_type = T;

	value_type x;
	value_type y;

	Point(const value_type x, const value_type y) : x(x), y(y) {}

	inline bool operator<(const Point& other) const
	{
		return (this->x < other.x) || ((this->x == other.x) && (this->y < other.y));
	}
};

struct RadiusDirection
{
	using value_type = size_t;
	enum class Direction : value_type
	{
		left = 0, right = 1
	};

	// The power of Direction set;
	constexpr static size_t size{ 2U };

private:
	Direction direction;
public:
	explicit RadiusDirection(const RadiusDirection::Direction direction) : direction(direction) {}

	static inline value_type transformation(const Direction direction)
	{
		return static_cast<value_type>(direction);
	}
};

template<class T>
struct Span
{
	using value_type = T;

public:
	value_type xoffset;
	RadiusDirection::Direction direction;

	Span() : xoffset(std::numeric_limits<value_type>::max()), direction(RadiusDirection::Direction::left) {}
	Span(const value_type xoffset, const RadiusDirection direction) : xoffset(xoffset), direction(direction) {}

	inline bool operator<(const Span<T>& other) const
	{
		return this->xoffset < other.xoffset;
	}

	inline void set(const value_type xoffset, const RadiusDirection::Direction direction)
	{
		this->xoffset = xoffset;
		this->direction = direction;
	}
};

class W1T4 : public Task
{
	using SpanT = Span<double>;
	using PointT = Point<double>;

	static constexpr double precision = 1e-4;

	std::vector<SpanT> buffer;

	void initialize(const size_t size)
	{
		buffer.resize(size + size);
	}

	int countPointsRadius(const std::vector<PointT>& points, const double radius);

	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W1T4() : Task(nullptr) {}
	explicit  W1T4(Test* const reference) : Task(reference) {}
};