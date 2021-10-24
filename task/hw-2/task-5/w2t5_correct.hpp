#pragma once
#include <iterator>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "../../../task.h"
#include "../../../test.h"

struct Path : private std::pair<int, bool>
{
	int& distance = std::pair<int, bool>::first;
	bool& isVisited = std::pair<int, bool>::second;

	Path() : std::pair<int, bool>()
	{;}

	Path(const int coordinate, const bool isVisited = false)
		: std::pair<int, bool>(coordinate, isVisited)
	{;}

	explicit Path(const Path& other)
		: std::pair<int, bool>(other.distance, other.isVisited)
	{;}

	Path(Path&& other) noexcept
		: std::pair<int, bool>(std::move(other))
	{;}
};

struct AliPoint : private std::pair<int, int>
{
	int& coordinate = std::pair<int, int>::first;
	int& lifetime = std::pair<int, int>::second;

	AliPoint() : std::pair<int, int>()
	{;}

	AliPoint(const int coordinate, const int lifetime)
		: std::pair<int, int>(coordinate, lifetime)
	{;}

	explicit AliPoint(const AliPoint& other)
		: std::pair<int, int>(other.coordinate, other.lifetime)
	{;}

	AliPoint(AliPoint&& other) noexcept
		: std::pair<int, int>(std::move(other))
	{;}

	inline AliPoint& operator=(const AliPoint& other)
	{
		this->lifetime = other.lifetime;
		this->coordinate = other.coordinate;

		return *this;
	}

	inline bool operator<(const AliPoint& other) const
	{
		return this->coordinate < other.coordinate;
	}
};

struct Explore
{
	using GraphTable = std::vector<std::vector<Path>>;

	std::vector<AliPoint> coins;

	GraphTable leftTable;
	GraphTable rightTable;

	explicit Explore(const size_t count) 
		: coins(count), 
		  leftTable(count, std::vector<Path>(count)), 
		  rightTable(count, std::vector<Path>(count))
	{;}

	int left(const int left, const int right);
	int right(const int left, const int right);

	int process();

	static inline int distance(const AliPoint& lhs, const AliPoint& rhs);

	static int check_vertex(const int res_left,
							const int add_left, 
							const int res_right, 
							const int add_right);
};

class W2T5 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W2T5() : Task(nullptr) {}
	explicit W2T5(Test* const reference) : Task(reference) {}
};
