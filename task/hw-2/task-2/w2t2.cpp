#include "w2t2.hpp"

#include <cstddef>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>

struct AssociatedValue : private std::pair<long long, long long>
{
	using LambdaOperator = const std::function<bool(const AssociatedValue&, const AssociatedValue&)>;

	static LambdaOperator less_value;
	static LambdaOperator less_index;

	long long& p_root = std::pair<long long, long long>::first;
	long long& index = std::pair<long long, long long>::second;

	AssociatedValue() : std::pair<long long, long long>() {}
	AssociatedValue(const long long& p_root, const long long& index) : std::pair<long long, long long>(p_root, index) {}

	AssociatedValue(AssociatedValue&& other) noexcept : std::pair<long long, long long>(std::move(other)) {}

	explicit AssociatedValue(const AssociatedValue& other) : std::pair<long long, long long>(other.p_root, other.index) {}

	AssociatedValue& operator= (const AssociatedValue& other)
	{
		p_root = other.p_root;
		index = other.index;

		return *this;
	}

	inline bool operator== (const AssociatedValue& other) const
	{
		return p_root == other.p_root;
	}
};

AssociatedValue::LambdaOperator AssociatedValue::less_value = [](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
	return lhs.p_root < rhs.p_root;
};

AssociatedValue::LambdaOperator AssociatedValue::less_index = [](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
	return lhs.index < rhs.index;
};

static long long leftBoundSearch(const AssociatedValue* pBegin, const AssociatedValue* pEnd, const long long offset, const long long p_root)
{
	const long long length = static_cast<long long>(pEnd - pBegin);

	long long left = offset;
	long long right = length - 1U;

	while (right - left > 1)
	{
		long long middle = (left + right) / 2;

		if (pBegin[middle].p_root <= p_root)
		{
			left = middle;
		}
		else
		{
			right = middle;
		}
	}

	while ((left + 1 < length) && (pBegin[left + 1].p_root <= p_root))
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

		++pBegin;
	} 
	while (pBegin < pEnd);
}

void W2T2::main(std::istream& input, std::ostream& output)
{
	std::istream_iterator<long long, char> istream(input);

	constexpr long long reserved_for_summ_0 = 1U;

	constexpr long long reserved = reserved_for_summ_0;

	const long long length = next(istream);

	if (length == 1)
	{
		output << next(istream) << ' ';
		output << 1 << '\n';

		return;
	}

	std::byte* pAllocation = new std::byte[sizeof(AssociatedValue) * length + sizeof(long long) * (length + reserved)];

	AssociatedValue* const pBeginValue = reinterpret_cast<AssociatedValue*>(pAllocation);
	AssociatedValue* const pEndValue = reinterpret_cast<AssociatedValue*>(pAllocation) + length;

	AssociatedValue* const pValues = pBeginValue;

	long long* const pSums = reinterpret_cast<long long*>(pEndValue);

	for (long long index = 0; index < length; ++index)
	{
		new(pBeginValue + index) AssociatedValue(next(istream), index + 1);
	}

	std::sort(pBeginValue, pEndValue, AssociatedValue::less_value);

	pSums[0] = 0;

	for (long long idx = 0; idx < length; ++idx)
	{
		pSums[reserved + idx] = pSums[idx] + pValues[idx].p_root;
	}

	long long max = pSums[0];

	AssociatedValue* pBeginIndex = pBeginValue;
	AssociatedValue* pEndIndex = pBeginValue;

	for (long long index = 0; index < length - 1; ++index)
	{
		const long long ai = pValues[index + 0U].p_root;
		const long long aj = pValues[index + 1U].p_root;

		const long long boundIdx = leftBoundSearch(pBeginValue, pEndValue, index, ai + aj);

		const long long hSum = pSums[boundIdx + 1U];
		const long long lSum = pSums[index];

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
		const long long count = 100'000;
		const long long commonElement = 1;
		const long long lastElement = count - 1;

		std::string input = std::to_string(count) + "\n";

		for (long long i = 0; i < count - 1; ++i)
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

		reference->open(*this).input("12 1 1 2 2 3 3 4 4 5 5 6 6").expect("36\n5 6 7 8 9 10 11 12 ");

		reference->open(*this).input("12 1 5 2 1 6 4 3 5 4 2 6 3").expect("36\n5 6 7 8 9 10 11 12 ");

		reference->open(*this).input("1 2147483647 2147483647 2147483647").expect("2147483648\n1 2 ");

	}
}