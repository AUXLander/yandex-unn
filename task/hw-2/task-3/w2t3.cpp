#include "w2t3.hpp"
#include <iterator>
#include <cstddef>


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
	return xPtr[xIdx] < yPtr[yIdx] ? xPtr[xIdx] : yPtr[yIdx];
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

		++dstIdx;
		++xIdx;
		++yIdx;
	}

	while (xIdx < x.size)
	{
		destination[dstIdx] = xPtr[xIdx];

		++dstIdx;
		++xIdx;
	}

	while (yIdx < y.size)
	{
		destination[dstIdx] = yPtr[yIdx];

		++dstIdx;
		++yIdx;
	}
}


void W2T3::main(std::istream& input, std::ostream& output)
{
	std::istream_iterator<int, char> istream(input);

	const size_t size = next(istream);
	const size_t length = next(istream);

	// крайние случаи с m = 0 || n = 0

	const size_t mainSize = size * length;
	const size_t tempSize = size * length;

	Container<int> container(tempSize + mainSize);

	// ===============================
	// |    temp     |     main      |
	// ===============================

	int* pTempArea = container.pBegin;
	int* pMainArea = container.pBegin + tempSize;

	for (size_t sequenceIdx = 0; sequenceIdx < size; ++sequenceIdx)
	{
		for (size_t elementIdx = 0; elementIdx < length; ++elementIdx)
		{
			pTempArea[elementIdx] = next(istream);
		}
	}

	for (size_t subsz = length; subsz < mainSize; subsz += subsz)
	{
		for (size_t stepsz = 0; stepsz < mainSize; stepsz += subsz)
		{
			int* const pLeft = pTempArea + stepsz;
			int* const pRight = pTempArea + stepsz + subsz;

			const Span<int> left(pLeft, subsz);
			const Span<int> right(pLeft, subsz);

			merge(left, right, pMainArea);
		}

		std::swap(pTempArea, pMainArea);
	}

	output << '\n' << '\n';

	for (size_t i = 0; i < mainSize + tempSize; ++i)
	{
		output << container.pBegin[i] << ' ';
	}
}

void W2T3::test(Test* const reference)
{
	
}
