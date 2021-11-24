#include "W3T3.h"

#include <deque>
#include <vector>
#include <list>
#include <algorithm>
#include <cstdio>
#include <iostream>

struct Segment 
{
	int start;
	int length;
	bool empty;
	int index;

	Segment(const int start = 0, const size_t length = 0,
		    const bool empty = false, const size_t index = 0)
		: start(start), length(length), 
		  empty(empty), index(index) 
	{;}

	~Segment()
	{;}

	inline bool operator== (const Segment& other) const
	{
		return (start == other.start) &&
			   (index == other.index) &&
			   (length == other.length) &&
			   (empty == other.empty);
	}

	inline bool operator< (const Segment& other) const 
	{
		return (length < other.length) ||
			   ((length == other.length) && (start > other.start));
	}

	inline bool operator> (const Segment& other) const
	{
		return other < *this;
	}
};

template<class T>
class HeapClassic 
{
	std::deque<T> buffer;

	void build(const int root_index) 
	{
		int left_index = 2 * root_index + 1;
		int right_index = 2 * root_index + 2;
		int index = root_index;

		if (left_index < static_cast<int>(buffer.size()) &&
			*(buffer[left_index]) > *(buffer[index]))
		{
			index = left_index;
		}

		if (right_index < static_cast<int>(buffer.size()) &&
			*(buffer[right_index]) > *(buffer[index]))
		{
			index = right_index;
		}

		if (index != root_index)
		{
			std::swap(buffer[root_index]->index, buffer[index]->index);
			std::swap(buffer[root_index], buffer[index]);

			build(index);
		}
	}

	void rebuild(int index, const T& new_key)
	{
		buffer[index] = new_key;
		new_key->index = index;
		build(index);

		while (index > 0 && *(buffer[(index - 1) / 2]) < *(buffer[index]))
		{
			std::swap(buffer[(index - 1) / 2]->index, buffer[index]->index);
			std::swap(buffer[(index - 1) / 2], buffer[index]);

			index = (index - 1) / 2;
		}
	}

	void exclude(const size_t index)
	{
		buffer[index] = buffer[buffer.size() - 1];
		buffer[index]->index = index;
		buffer.pop_back();
	}

public:
	HeapClassic() 
	{;}

	~HeapClassic() 
	{;}

	inline bool empty() const 
	{
		return buffer.empty();
	}

	void erase(const int index)
	{
		if (buffer.empty() == false)
		{
			exclude(index);
			rebuild(index, buffer[index]);
		}
	}

	void insert(const T& new_key) 
	{
		buffer.push_back(new_key);
		rebuild(buffer.size() - 1, new_key);
	}

	T pop_max()
	{
		auto max_element = buffer[0];

		exclude(0);
		build(0);

		return max_element;
	}

	T max() 
	{
		return buffer[0];
	}
};

class Manager 
{
	using ChunkRef = std::list<Segment>::iterator;

	int transaction_id;
	std::list<Segment> memory;
	HeapClassic<ChunkRef> memspace;
	std::deque<ChunkRef> memalloc;

public:
	explicit Manager(int size)
	{
		transaction_id = 0;
		memory.emplace_back(1, size, true, 0);
		memspace.insert(memory.begin());
	}

	~Manager()
	{;}

	int allocate(int size)
	{
		++transaction_id;
		memalloc.resize(transaction_id, memory.end());

		if (memspace.empty())
		{
			return -1;
		}

		if (memspace.max()->length < size)
		{
			return -1;
		}

		auto free_chunk = memspace.pop_max();

		memalloc[transaction_id - 1] = memory.emplace(free_chunk, free_chunk->start, size, false);

		if (free_chunk->length == size)
		{
			memory.erase(free_chunk);
			return memalloc[transaction_id - 1]->start;
		}

		free_chunk->length -= size;
		free_chunk->start += size;
		memspace.insert(free_chunk);

		return memalloc[transaction_id - 1]->start;
	}

	void free(int id)
	{
		++transaction_id;

		if (memalloc[id - 1] == memory.end())
		{
			return;
		}

		memalloc[id - 1]->empty = true;

		if (memalloc[id - 1] != memory.begin())
		{
			auto prev_chunk = memalloc[id - 1];
			--prev_chunk;

			if (prev_chunk->empty)
			{
				memalloc[id - 1]->length += prev_chunk->length;
				memalloc[id - 1]->start = prev_chunk->start;
				memspace.erase(prev_chunk->index);
				memory.erase(prev_chunk);
			}
		}

		if (memalloc[id - 1] != --(memory.end()))
		{
			auto next_chunk = memalloc[id - 1];
			++next_chunk;

			if (next_chunk->empty)
			{
				memalloc[id - 1]->length += next_chunk->length;
				memspace.erase(next_chunk->index);
				memory.erase(next_chunk);
			}
		}

		memspace.insert(memalloc[id - 1]);
	}
};

void W3T3::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("6 8 2 3 -1 3 3 -5 2 2 ").expect("1\n3\n-1\n-1\n1\n-1\n");
	}
}

void W3T3::main(std::istream& input, std::ostream& output)
{
	int size, count;

	input >> size >> count;

	Manager memory(size);

	std::vector<int> output_vec;
	std::vector<int> requests(count);

	for (auto& request : requests)
	{
		input >> request;
	}

	for (const auto& request : requests)
	{
		if (request > 0)
		{
			const auto response = memory.allocate(request);

			output_vec.emplace_back(response);
		}

		if (request < 0)
		{
			memory.free(-request);
		}
	}

	for (const auto& element : output_vec)
	{
		output << element << '\n';
	}
}