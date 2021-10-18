#include "w2t5.hpp"
#include <vector>
#include <stack>
#include <algorithm>

struct Vertex;

template<class T>
class PlaceFixedList
{
	friend class Vertex;

	struct Node;

	const size_t cap;
	size_t sz;

	Node* pAllocation;
	Node* pFirst;
	Node* pLast;

	struct Node
	{
		PlaceFixedList& list;

		T* value;

		bool isVisible;

		Node* prev;
		Node* next;

		Node(PlaceFixedList& list, T* value, const bool isVisible = false, Node* prev = nullptr)
			: list(list), value(value), isVisible(isVisible), prev(prev), next(nullptr)
		{
			if (prev != nullptr)
			{
				prev->next = this;
			}
		}

		~Node() {}

		void hide()
		{
			isVisible = false;

			if (prev != nullptr)
			{
				prev->next = next;
			}
			else
			{
				list.pFirst = next;
			}

			if (next != nullptr)
			{
				next->prev = prev;
			}
			else
			{
				list.pLast = prev;
			}

			--list.sz;
		}

		void show()
		{
			isVisible = true;

			if (prev != nullptr)
			{
				prev->next = this;
			}
			else
			{
				list.pFirst = this;
			}

			if (next != nullptr)
			{
				next->prev = this;
			}
			else
			{
				list.pLast = this;
			}

			++list.sz;
		}
	};

public:
	explicit PlaceFixedList(std::vector<T>& container)
		: cap(container.size()), sz(0),
		  pAllocation(nullptr), pFirst(nullptr), pLast(nullptr)
	{
		pAllocation = reinterpret_cast<Node*>(new std::byte[sizeof(Node) * cap]);

		size_t index = 0;

		Node* pPrevious = nullptr;

		if (cap > 0)
		{
			pFirst = pPrevious = new(pAllocation) Node(*this, container.data() + index, false, nullptr);

			for (index = 1; index < cap; ++index)
			{
				pPrevious = new(pAllocation + index) Node(*this, container.data() + index, false, pPrevious);
			}

			if (cap > 0)
			{
				pLast = pPrevious;
			}

			//if ((pLast != nullptr) && (pFirst != nullptr))
			//{
			//	pLast->next = pFirst;
			//	pFirst->prev = pLast;
			//}
		}

		sz = index;
	}

	Node* first()
	{
		return pFirst;
	}

	Node* last()
	{
		return pLast;
	}

	size_t size() const
	{
		return sz;
	}

	size_t capacity() const
	{
		return cap;
	}

	~PlaceFixedList()
	{
		delete[] pAllocation;
	}
};

struct Point
{
	int xx;
	int yy;

	Point(int x, int y) : xx(x), yy(y) {};
};


struct Vertex
{
	Point point;
	
	bool isVisited;
	
	PlaceFixedList<Vertex>::Node* pNode;

	explicit Vertex(const Point& point, const bool visit = false, PlaceFixedList<Vertex>::Node* pNode = nullptr)
		: point(point), isVisited(visit), pNode(pNode) {}

	void reset()
	{
		isVisited = false;
	}

	void setNode(PlaceFixedList<Vertex>::Node* pNode)
	{
		this->pNode = pNode;
	}

	static int distance(const Vertex& first, const Vertex& second)
	{
		return std::abs(first.point.xx - second.point.xx);
	}

	void visit()
	{
		if (pNode != nullptr)
		{
			pNode->hide();
		}

		isVisited = true;
	}

	void unvisit()
	{
		if (pNode != nullptr)
		{
			pNode->show();
		}

		isVisited = false;
	}

	Vertex& operator=(const Vertex& other)
	{
		point = other.point;
		isVisited = other.isVisited;

		return *this;
	}
};

class VertexBuffer
{
	size_t index = 0;
	size_t locked_count = 0;

	PlaceFixedList<Vertex> cache;

public:
	explicit VertexBuffer(std::vector<Vertex>& map) : cache(map)
	{
		auto* pNode = cache.first();

		if (pNode != nullptr)
		{
			do
			{
				pNode->value->setNode(pNode);
				pNode = pNode->next;
			} 
			while ((pNode != nullptr) && (pNode->next != nullptr));

			pNode->value->setNode(pNode);
		}

		pNode = pNode;
	}

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

	Vertex* next(const size_t offset)
	{
		index = 0;
		size_t sub_offset = 0;

		auto* pNode = cache.first();

		if (pNode != nullptr)
		{
			for (; (sub_offset < offset); ++sub_offset)
			{
				if (pNode->next == nullptr)
				{
					if (pNode == pNode->list.last())
					{
						pNode = pNode->list.first();
					}
					else
					{
						return nullptr;
					}
				}
				else
				{
					pNode = pNode->next;
				}
			}
			
			return pNode->value;
		}

		return nullptr;
	}

	size_t avaible() const
	{
		return cache.capacity() - locked_count;
	}
};

constexpr int invalid_time_value = std::numeric_limits<int>::max();

int research(VertexBuffer& container, Vertex& element, const int time)
{
	container.lock(element);

	int min_time = invalid_time_value;
	const size_t avaible_count = container.avaible();

	for (size_t offset = 0; offset < avaible_count; ++offset)
	{
		auto* pNext = container.next(offset);

		if (pNext != nullptr)
		{
			auto& next = *pNext;

			auto distance = Vertex::distance(next, element);

			if (next.point.yy - distance - time >= 0)
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
		else
		{
			min_time = time;

			break;
		}
	}

	if (avaible_count == 0)
	{
		min_time = time;
	}

	container.unlock(element);

	return min_time;
}

void W2T5::main(std::istream& input, std::ostream& output)
{
	//std::istream_iterator<int, char> istream(input);

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

	int N;// = next(istream);

	input >> N;

	std::vector<Point> points;

	points.reserve(N);

	for (int i = 0; i < N; i++)
	{
		int x, y;

		input >> x >> y;

		points.emplace_back(x, y);

		//points.emplace_back(i, 1000 - i);
		///std::cout << i << ' ' << 1000 - i << ' ';
	}

	std::vector<Vertex> map;

	map.reserve(points.size());

	for (const auto& point : points)
	{
		map.emplace_back(point);
	}

	std::sort(map.begin(), map.end(), [](const Vertex& lhs, const Vertex& rhs) { return lhs.point.yy < rhs.point.yy; });
	
	VertexBuffer container(map);
	
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