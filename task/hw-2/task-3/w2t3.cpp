#include "w2t3.hpp"
#include <iterator>
#include <algorithm>
#include <cstddef>
#include <vector>
#include <string>
#include <random>

template<class T>
class Container
{
	std::byte* const pAllocation;
public:
	const size_t size;

	T* const pBegin = reinterpret_cast<T* const>(pAllocation);
	T* const pEnd = reinterpret_cast<T* const>(pAllocation) + size;

	Container(const size_t size) : size(size), pAllocation(new std::byte[sizeof(T) * size])
	{

	}

	inline T& operator[](const size_t index)
	{
		return *(reinterpret_cast<T*>(pAllocation) + index);
	}

	~Container()
	{
		delete[] pAllocation;
	}
};

template<class T>
struct Span : private std::pair<T* const, const size_t>
{
	T* const pointer = std::pair<T* const, const size_t>::first;
	const size_t size = std::pair<T* const, const size_t>::second;

	Span(T* const pointer, const size_t size) : std::pair<T* const, const size_t>(pointer, size) {}
	Span(const Span& other) : std::pair<T* const, const size_t>(other.pointer, other.size) {}
	Span(Span&& other) : std::pair<T* const, const size_t>(std::move(other)) {}
};

template<class T>
inline T& selector(T* const xPtr, size_t& xIdx, T* const yPtr, size_t& yIdx)
{
	return xPtr[xIdx] < yPtr[yIdx] ? xPtr[xIdx++] : yPtr[yIdx++];
}

bool passed = true;

void stest(int* pFirst, int* pSecond, const int size)
{
	if (passed == true)
	{
		for (int i = 0; i < size; ++i)
		{
			if (pSecond + size == std::find(pSecond, pSecond + size, pFirst[i]))
			{
				passed = false;

				return;
			}
		}
	}
}

template<class T>
void merge(const Span<T> x, const Span<T> y, T* const destination)
{
	size_t xIdx = 0;
	size_t yIdx = 0;
	size_t dstIdx = 0;

	T* const xPtr = x.pointer;
	T* const yPtr = y.pointer;

	while ((xIdx < x.size) && (yIdx < y.size))
	{
		destination[dstIdx] = selector(xPtr, xIdx, yPtr, yIdx);

		dstIdx++;
	}

	while (xIdx < x.size)
	{
		destination[dstIdx] = xPtr[xIdx++];

		dstIdx++;
	}

	while (yIdx < y.size)
	{
		destination[dstIdx] = yPtr[yIdx++];

		dstIdx++;
	}
}

void print(std::ostream& output, const int* pIndex, const size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		output << pIndex[i] << ' ';
	}

	//output << '\n';
}

void W2T3::main(std::istream& input, std::ostream& output)
{
	std::istream_iterator<int, char> istream(input);

	const size_t size = next(istream);
	const size_t length = next(istream);

	if (size == 1)
	{
		for (size_t elementIdx = 0; elementIdx < size * length; ++elementIdx)
		{
			output << next(istream);
		}

		return;
	}
	else if (length == 1)
	{
		Container<int> container(size);

		for (size_t elementIdx = 0; elementIdx < size * length; ++elementIdx)
		{
			container.pBegin[elementIdx] = next(istream);
		}

		std::sort(container.pBegin, container.pEnd);

		print(output, container.pBegin, size * length);
	
		return;
	}

	const int total = size * length;

	//Container<int> container(total + total); container.pBegin

	std::vector<int> container(total + total, 0);

	// ===============================
	// |    temp     |     main      |
	// ===============================

	int* pMemory = container.data();
	int* pSwap = container.data() + total;

	for (size_t sequenceIdx = 0; sequenceIdx < size; ++sequenceIdx)
	{
		for (size_t elementIdx = 0; elementIdx < length; ++elementIdx)
		{
			pSwap[elementIdx + sequenceIdx * length] = 
				pMemory[elementIdx + sequenceIdx * length] = next(istream);
		}
	}

	int offset = 0;
	int subhl = length;
	int subsz = subhl + subhl;

	int s;

	while (subhl <= total)
	{
		offset = 0;
		s = offset;

		stest(pSwap, pMemory, total);
		stest(pMemory, pSwap, total);

		while (offset + subsz <= total)
		{
			const auto lsz = std::min(total, subhl);
			const auto rsz = std::min(total - lsz, subhl);

			int* const pLeft = pMemory + offset;
			int* const pRight = pMemory + offset + lsz;

			const Span<int> left(pLeft, lsz);
			const Span<int> right(pRight, rsz);

			merge(left, right, pSwap + offset);

			s = offset;

			offset += lsz + rsz;
		}

		stest(pSwap, pMemory, total);
		stest(pMemory, pSwap, total);

		if (offset + subhl < total)
		{
			const auto lsz = subhl;
			const auto rsz = total - offset - lsz;

			int* const pLeft = pMemory + offset;
			int* const pRight = pMemory + offset + lsz;

			const Span<int> left(pLeft, lsz);
			const Span<int> right(pRight, rsz);

			stest(pSwap, pMemory, total);
			stest(pMemory, pSwap, total);

			merge(left, right, pSwap + offset);

			stest(pSwap, pMemory, total);
			stest(pMemory, pSwap, total);

			//std::swap(pSwap, pMemory);
		}
		else if (offset < total)
		{
			stest(pSwap, pMemory, total);
			stest(pMemory, pSwap, total);

			std::copy(pMemory + offset, pMemory + total, pSwap + offset);

			stest(pSwap, pMemory, total);
			stest(pMemory, pSwap, total);
			//std::swap(pSwap, pMemory);
		}



		subhl = subsz;
		subsz += subsz;

		std::swap(pSwap, pMemory);
	}

	//std::swap(pSwap, pMemory);

	//std::swap(pSwap, pMemory);

	print(output, pMemory, total);
}

void W2T3::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("5 2 1 4 2 8 3 7 9 10 5 6\n").expect("1 2 3 4 5 6 7 8 9 10 ");
		reference->open(*this).input("4 2 1 4 2 8 3 7 5 6\n").expect("1 2 3 4 5 6 7 8 ");
		reference->open(*this).input("2 5 1 3 5 7 9 2 4 6 8 10\n").expect("1 2 3 4 5 6 7 8 9 10 ");
		reference->open(*this).input("10 1 1 3 5 7 9 2 4 6 8 10\n").expect("1 2 3 4 5 6 7 8 9 10 ");
		reference->open(*this).input("9 1 1 3 5 7 9 2 4 6 8\n").expect("1 2 3 4 5 6 7 8 9 ");

		size_t nSize = 7;
		size_t mSize = 7;

		std::vector<int> marr;
		std::string input;
		std::string output;

		std::srand(100);

		marr.reserve(nSize * mSize);

		input.reserve((std::log(1000) + 1) * nSize);

		input += std::to_string(nSize) + ' ' + std::to_string(mSize) + ' ';

		for (size_t nIdx = 0; nIdx < nSize; ++nIdx)
		{
			std::vector<int> t_marr;

			t_marr.reserve(mSize);

			for (size_t mIdx = 0; mIdx < mSize; ++mIdx)
			{
				const int value = static_cast<int>(100 * static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));

				marr.emplace_back(value);
				t_marr.emplace_back(value);
			}

			std::sort(t_marr.begin(), t_marr.end());

			for (const auto value : t_marr)
			{
				input += std::to_string(value) + ' ';
			}
		}

		std::sort(marr.begin(), marr.end());

		output.reserve(input.size());

		for (const auto el : marr)
		{
			output += std::to_string(el) + ' ';
		}

		reference->open(*this).input(input).expect(output, -1, true);
	}
}
