#include "w2t2.hpp"

#include <cstddef>
#include <utility>
#include <algorithm>
#include <functional>

struct AssociatedValue : private std::pair<int, int>
{
	using LambdaOperator = const std::function<bool(const AssociatedValue&, const AssociatedValue&)>;

	static LambdaOperator less_value;
	static LambdaOperator less_index;

	int& value = std::pair<int, int>::first;
	int& index = std::pair<int, int>::second;

	AssociatedValue() : std::pair<int, int>() {}
	AssociatedValue(const int& value, const int& index) : std::pair<int, int>(value, index) {}

	AssociatedValue(AssociatedValue&& other) noexcept : std::pair<int, int>(std::move(other)) {}

	explicit AssociatedValue(const AssociatedValue& other) : std::pair<int, int>(other.value, other.index) {}

	AssociatedValue& operator= (const AssociatedValue& other)
	{
		value = other.value;
		index = other.index;

		return *this;
	}

	inline bool operator== (const AssociatedValue& other) const
	{
		return value == other.value;
	}
};

AssociatedValue::LambdaOperator AssociatedValue::less_value = [](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
	return lhs.value < rhs.value;
};

AssociatedValue::LambdaOperator AssociatedValue::less_index = [](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
	return lhs.index < rhs.index;
};

static int leftBoundSearch(const AssociatedValue* pBegin, const AssociatedValue* pEnd, const int offset, const int value)
{
	const int length = static_cast<int>(pEnd - pBegin);

	int left = offset;
	int right = length - 1U;

	while (right - left > 1)
	{
		int middle = (left + right) / 2;

		if (pBegin[middle].value <= value)
		{
			left = middle;
		}
		else
		{
			right = middle;
		}
	}
	
	const int restored = left;

	while ((left + 1 < length) && (pBegin[left + 1].value <= value))
	{
		++left;
	}

	return left;
}

static void printIndex(std::ostream& output, const AssociatedValue* pBegin, const AssociatedValue* const pEnd)
{
	do
	{
		output << pBegin->index << ' ';
	} 
	while (++pBegin != pEnd);
}

void W2T2::main(std::istream& input, std::ostream& output)
{
	std::istream_iterator<int, char> istream(input);

	constexpr int reserved_for_summ_0 = 1U;

	constexpr int reserved = reserved_for_summ_0;

	const int length = next(istream);

	if (length <= 0)
	{
		output << 0 << '\n';

		return;
	}

	std::byte* pAllocation = new std::byte[sizeof(AssociatedValue) * length + sizeof(int) * (length + reserved)];

	AssociatedValue* const pBeginValue = reinterpret_cast<AssociatedValue*>(pAllocation);
	AssociatedValue* const pEndValue = reinterpret_cast<AssociatedValue*>(pAllocation) + length;

	AssociatedValue* const pValues = pBeginValue;

	int* const pSums = reinterpret_cast<int*>(pEndValue);

	for (int index = 0; index < length; ++index)
	{
		new(pBeginValue + index) AssociatedValue(next(istream), index + 1);
	}

	std::sort(pBeginValue, pEndValue, AssociatedValue::less_value);

	pSums[0] = 0;

	for (int idx = 0; idx < length; ++idx)
	{
		pSums[reserved + idx] = pSums[idx] + pValues[idx].value;
	}

	int max = pSums[0];

	AssociatedValue* pBeginIndex = pBeginValue;
	AssociatedValue* pEndIndex = pBeginValue;

	for (int index = 0; index < length - 1; ++index)
	{
		const int ai = pValues[index + 0U].value;
		const int aj = pValues[index + 1U].value;

		const int boundIdx = leftBoundSearch(pBeginValue, pEndValue, index, ai + aj);

		const int hSum = pSums[boundIdx + 1U];
		const int lSum = pSums[index];

		if (hSum - lSum > max)
		{
			max = hSum - lSum;

			pBeginIndex = pBeginValue + index ;
			pEndIndex = pBeginValue + boundIdx + 1;
		}
	}

	std::sort(pBeginIndex, pEndIndex, AssociatedValue::less_index);

	output << max << '\n';

	printIndex(output, pBeginIndex, pEndIndex);

	delete[] pAllocation;
}

void W2T2::test(Test* const reference)
{
	if (reference != nullptr)
	{
		const int count = 100000;
		const int commonElement = 1;
		const int lastElement = count + 1;

		std::string input = std::to_string(count) + "\n";

		for (int i = 0; i < count - 1; ++i)
		{
			input += std::to_string(commonElement) + ' ';
		}

		input += std::to_string(lastElement);

		reference->open(*this).input(input).expect(std::to_string(lastElement + commonElement) + "\n" + std::to_string(count - 1) + ' ' + std::to_string(count) + ' ');

		reference->open(*this).input("5 1 1 1 1 1").expect("5\n1 2 3 4 5 ");
		reference->open(*this).input("5 1 1 1 1 30").expect("31\n4 5 ");
		reference->open(*this).input("5 1 1 1 30 1").expect("31\n3 4 ");
		reference->open(*this).input("6 1 1 1 1 1 4").expect("5\n1 2 3 4 5 ");
		
		reference->open(*this).input("6 8 2 1 3 4 5").expect("17\n1 5 6 ");
		reference->open(*this).input("10 8 1 1 1 1 1 1 1 1 1").expect("9\n1 2 ");

	}
}