#pragma once
#include <iterator>
#include <iostream>
#include <vector>


#include "../../../task.h"
#include "../../../test.h"

struct Point : private std::pair<double, double>
{
	double& coord_x = std::pair<double, double>::first;
	double& coord_y = std::pair<double, double>::second;

	Point() : std::pair<double, double>() {}
	Point(const double x, const double y) : std::pair<double, double>(x, y) {}

	inline bool operator<(const Point& other) const
	{
		return (this->coord_x < other.coord_x) || ((this->coord_x == other.coord_x) && (this->coord_y < other.coord_y));
	}
};

using BoundType = bool;

struct Bound : private std::pair<double, BoundType>
{
	double& xoffset = std::pair<double, BoundType>::first;
	BoundType& type = std::pair<double, BoundType>::second;

	Bound() : std::pair<double, BoundType>() {}
	Bound(const double xoffset, const BoundType type) 
		: std::pair<double, BoundType>(xoffset, type) {}

	Bound(Bound&& other) noexcept : std::pair<double, BoundType>(std::move(other)) {}

	inline bool operator<(const Bound& other) const
	{
		return this->xoffset < other.xoffset;
	}

	inline Bound& operator=(const Bound& other)
	{
		xoffset = other.xoffset;
		type = other.type;

		return *this;
	}
};

class W1T4 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W1T4() : Task(nullptr) {}
	explicit  W1T4(Test* const reference) : Task(reference) {}
};