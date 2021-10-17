#include "w2t5.hpp"
#include <vector>
#include <stack>
#include <algorithm>


struct Point
{
	int x;
	int y;

	Point(int x, int y) : x(x), y(y) {};
};

struct Vertex
{
	Point point;
	
	bool isVisited;

	explicit Vertex(const Point& point, const bool visit = false)
		: point(point), isVisited(visit) {}

	void reset()
	{
		isVisited = false;
	}

	static int distance(const Vertex& first, const Vertex& second)
	{
		return std::abs(first.point.x - second.point.x);
	}

	void visit()
	{
		isVisited = true;
	}

	void unvisit()
	{
		isVisited = false;
	}

	Vertex& operator=(const Vertex& other)
	{
		point = other.point;
		isVisited = other.isVisited;

		return *this;
	}
};

struct VertexBuffer
{
	size_t locked_count = 0;

	std::vector<Vertex> map;

	void lock(Vertex& element)
	{
		element.visit();
		++locked_count;
	}

	void unlock(Vertex& element)
	{
		element.unvisit();
		--locked_count;
	}

	Vertex& next(const size_t offset)
	{
		size_t index = 0;
		size_t sub_offset = 0;

		while (index < map.size())
		{
			if (map[index].isVisited == true)
			{
				++index;

				continue;
			}
			else if (sub_offset == offset)
			{
				return map[index];
			}
			else if (sub_offset < offset)
			{
				++index;
				++sub_offset;
			}
		}

		//error
		return map[index];
	}

	size_t avaible() const
	{
		return map.size() - locked_count;
	}
};

constexpr int invalid_time_value = std::numeric_limits<int>::max();

int research(VertexBuffer& container, Vertex& element, const int time)
{
	container.lock(element);

	int min_time = invalid_time_value;
	const size_t avaible_count = container.avaible();

	if (avaible_count > 0)
	{
		for (size_t offset = 0; offset < avaible_count; ++offset)
		{
			auto& next = container.next(offset);
			auto  distance = Vertex::distance(next, element);

			if (next.point.y - distance - time >= 0)
			{
				const int new_time = research(container, next, time + distance);

				if (new_time >= 0)
				{
					min_time = std::min(min_time, new_time);
				}
			}
			else
			{
				container.unlock(element);

				return invalid_time_value;
			}
		}
	}
	else
	{
		min_time = time;
	}

	container.unlock(element);

	return min_time;
}



int research(std::vector<Vertex>& map, Vertex& parent, const int time)
{
	bool found = false;
	int min_time = std::numeric_limits<int>::max();

	for (auto& vertex : map)
	{
		if (vertex.isVisited == false)
		{
			found = true;

			int distance = Vertex::distance(vertex, parent);

			int loctime = vertex.point.y - distance - time;

			if (loctime >= 0)
			{
				vertex.visit();

				int ztime = research(map, vertex, time + distance);

				vertex.unvisit();

				if (ztime >= 0)
				{
					min_time = std::min(min_time, ztime);
				}
			}
			else
			{
				return -1;
			}
		}
	}

	return found ? min_time : time;
}

void W2T5::main(std::istream& input, std::ostream& output)
{
	//const std::vector<Point> points
	//{
	//	{1, 7},
	//	{2, 2},
	//	{4, 2},
	//	{5, 3}
	//};

	//const std::vector<Point> points
	//{
	//	{1,  3},
	//	{3,  1},
	//	{5,  8},
	//	{8,  19},
	//	{10, 15},
	//};

	//const std::vector<Point> points
	//{
	//	{1, 5},
	//	{2, 1},
	//	{3, 4},
	//	{4, 2},
	//	{5, 3},
	//};

	std::vector<Point> points;

	points.reserve(100);

	for (int i = 0; i < 100; i++)
	{
		points.emplace_back(i, 100 - i);
	}

	VertexBuffer container;

	std::vector<Vertex>& map = container.map;

	map.reserve(points.size());

	for (const auto& point : points)
	{
		map.emplace_back(point);
	}

	std::sort(map.begin(), map.end(), [](const Vertex& lhs, const Vertex& rhs) { return lhs.point.y < rhs.point.y; });

	int min_time = invalid_time_value;

	for (auto& vertex : map)
	{
		int time = research(container, vertex, 0);

		if (time >= 0)
		{
			min_time = std::min(time, min_time);
		}
	}

	std::cout << min_time;
}

void W2T5::test(Test* const reference)
{

};