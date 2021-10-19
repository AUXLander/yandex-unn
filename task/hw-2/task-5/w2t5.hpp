#pragma once
#include <iterator>
#include <iostream>
#include <vector>
#include <algorithm>

#include "../../../task.h"
#include "../../../test.h"

/*
*
*	Longest switchable subsequence
*
*/

struct Vertex;
struct PlaceFixedList;

struct Node
{
	PlaceFixedList& list;

	Vertex* value;

	bool isVisible;

	Node* pPrev;
	Node* pNext;

	Node(PlaceFixedList& list, Vertex* value, const bool isVisible = false, Node* prev = nullptr)
		: list(list), value(value), isVisible(isVisible), pPrev(prev), pNext(nullptr)
	{}

	void hide();
	void show();
};

struct Vertex
{
	int xx;
	int yy;

	Node* pNode;

	explicit Vertex(const int xx, const int yy, const bool visit = false, Node* pNode = nullptr);

	inline void setNode(Node* pNode);

	inline static int distance(const Vertex& first, const Vertex& second);

	inline void visit();

	inline void unvisit();

	inline Vertex& operator=(const Vertex& other);
};

class PlaceFixedList
{
	friend class Node;
	friend class Vertex;

	const size_t cap;
	size_t sz;

	char* pAllocationB;

	Node* pAllocation;
	Node* pFirst;
	Node* pLast;

public:
	explicit PlaceFixedList(std::vector<Vertex>& container)
		: cap(container.size()), sz(0),
		pAllocationB(nullptr), pAllocation(nullptr), pFirst(nullptr), pLast(nullptr)
	{
		pAllocationB = new char[sizeof(Node) * cap];

		pAllocation = reinterpret_cast<Node*>(pAllocationB);

		size_t index = 0;

		Node* pPrevious = nullptr;

		if (cap > 0)
		{
			pFirst = pPrevious = new(pAllocation) Node(*this, container.data() + index, false, nullptr);

			pPrevious->value->pNode = pPrevious;

			for (index = 1; index < cap; ++index)
			{
				pPrevious = new(pAllocation + index) Node(*this, container.data() + index, false, pPrevious);

				pPrevious->value->pNode = pPrevious;
			}

			if (cap > 0)
			{
				pLast = pPrevious;
			}
		}

		sz = index;
	}

	inline Node* first()
	{
		return pFirst;
	}

	inline Node* last()
	{
		return pLast;
	}

	inline size_t size() const
	{
		return sz;
	}

	inline size_t capacity() const
	{
		return cap;
	}

	~PlaceFixedList()
	{
		delete[] pAllocationB;
	}
};

class VertexBuffer
{
	size_t locked_count = 0;

	PlaceFixedList cache;

public:
	explicit VertexBuffer(std::vector<Vertex>& map) : cache(map) {}

	void lock(Vertex& element);
	void unlock(Vertex& element);

	Vertex* next(const size_t offset);
	
	size_t avaible() const
	{
		return cache.capacity() - locked_count;
	}
};

class W2T5 : public Task
{
	void test(Test* const reference) override final;
	void main(std::istream& input, std::ostream& output) override final;

public:
	W2T5() : Task(nullptr) {}
	explicit W2T5(Test* const reference) : Task(reference) {}
};