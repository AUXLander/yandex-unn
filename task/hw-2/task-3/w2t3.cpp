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
	std::byte* const p_allocation;
public:
	const int size;

	T* const pBegin = reinterpret_cast<T* const>(p_allocation);
	T* const pEnd = reinterpret_cast<T* const>(p_allocation) + size;

	Container(const int size) : p_allocation(new std::byte[sizeof(T) * size]), size(size)
	{

	}

	inline T* data()
	{
		return pBegin;
	}

	inline T& operator[](const int index)
	{
		return *(reinterpret_cast<T*>(p_allocation) + index);
	}

	~Container()
	{
		delete[] p_allocation;
	}
};

size_t operations = 0;;

template<class T>
void merge(T* xPtr, const int xSize, T* yPtr, const int ySize, T* destination)
{
	T* xPtrEnd = xPtr + xSize;
	T* yPtrEnd = yPtr + ySize;

	while ((xPtr < xPtrEnd) && (yPtr < yPtrEnd))
	{
		if ((*xPtr) < (*yPtr))
		{
			*destination = *xPtr;

			++xPtr;
		}
		else
		{
			*destination = *yPtr;

			++yPtr;
		}

		++destination;
		++operations;
	}

	operations += xPtrEnd - xPtr;
	operations += yPtrEnd - yPtr;

	std::copy(xPtr, xPtrEnd, destination);
	std::copy(yPtr, yPtrEnd, destination);
}


void W2T3::main(std::istream& input, std::ostream& output)
{
	std::istream_iterator<int, char> istream(input);

	const int size = next(istream);
	const int length = next(istream);

	const int total = size * length;

	if (size == 1)
	{
		for (int elementIdx = 0; elementIdx < size * length; ++elementIdx)
		{
			output << next(istream) << ' ';
		}

		return;
	}

	// ===============================
	// |    swap     |     main      |
	// ===============================

	Container<int> container(total + total);

	int* pMemory = container.data();
	int* pSwap = container.data() + total;

	for (size_t index = 0; index < total; ++index)
	{
		pSwap[index] = pMemory[index] = next(istream);

		++operations;
	}

	int offset = 0;
	int subhl = length;
	int subsz = subhl + subhl;

	while (subhl <= total)
	{
		offset = 0;

		while (offset + subsz <= total)
		{
			const auto lsz = std::min(total, subhl);
			const auto rsz = std::min(total - lsz, subhl);

			int* const pLeft = pMemory + offset;
			int* const pRight = pMemory + offset + lsz;

			merge(pLeft, lsz, pRight, rsz, pSwap + offset);

			offset += lsz + rsz;
		}

		if (offset + subhl < total)
		{
			const auto lsz = subhl;
			const auto rsz = total - offset - lsz;

			int* const pLeft = pMemory + offset;
			int* const pRight = pMemory + offset + lsz;

			merge(pLeft, lsz, pRight, rsz, pSwap + offset);
		}
		else if (offset < total)
		{
			std::copy(pMemory + offset, pMemory + total, pSwap + offset);

			operations += total - offset;
		}

		subhl = subsz;
		subsz += subsz;

		std::swap(pSwap, pMemory);
	}

	for (size_t index = 0; index < total; ++index)
	{
		output << pMemory[index] << ' ';
	}
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

		reference->open(*this).input("2 5 1 1 1 1 1 1 1 1 1 1\n").expect("1 1 1 1 1 1 1 1 1 1 ");

		reference->open(*this).input("2 3 1 1 1 2 2 2\n").expect("1 1 1 2 2 2 ");
		reference->open(*this).input("2 5 1 2 2 3  4 2 3 4 4 5\n").expect("1 2 2 2 3 3 4 4 4 5 ");

		std::srand(100);

		const auto test = [&](size_t nSize, size_t mSize)
		{
			std::vector<int> marr;
			std::string input;
			std::string output;


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

			operations = 0;

			return reference->open(*this).input(input).expect(output, -1, true);

			//std::cout << std::endl << operations;
		};


		bool play = true;

		while (play)
		{
			const int count = 1 + static_cast<int>(999 * static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));
			const int length = 1 + static_cast<int>(999 * static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));

			play = test(count, length);
		}
	}
}
