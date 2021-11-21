#include "W3T5.h"
#include <tuple>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
#include <algorithm>

constexpr size_t kGRandMax = 2147483647;


template<typename T, typename H>
struct signature_t
{
	using triplet = std::tuple<H, H, H>;

	const triplet sign;

	signature_t(const T a, const T b, const T c)
		: sign(this->process(a, b, c))
	{;}

	signature_t(const signature_t& other)
		: sign(other.sign)
	{;}

	signature_t(signature_t&& other) 
		: sign(std::move(other.sign))
	{;}

	~signature_t()
	{;}

	inline bool operator== (const signature_t<T, H>& other) const
	{
		return sign == other.sign;
	}

	inline bool operator< (const signature_t<T, H>& other) const
	{
		H la, lb, lc;
		H ld, le, lf;

		std::tie(la, lb, lc) = this->sign;
		std::tie(ld, le, lf) = other.sign;

		if ((la < ld)
			|| ((la == ld) && (lb < le))
			|| ((lb == le) && (lc < lf)))
		{
			return true;
		}

		return false;
	}

	triplet process(T, T, T)
	{
		return triplet();
	};
};

using sign_key_type = unsigned int;
using signature = signature_t<sign_key_type, float>;

class signature_table
{
public:
	using Node = std::pair<signature, size_t>;

	using bucket = std::list<Node>;

private:
	size_t size;
	std::vector<std::unique_ptr<bucket>> htable;

public:
	explicit signature_table(const size_t size = 1000)
		: size(size), htable(size)
	{;}

	~signature_table()
	{;}

	size_t hash_index(const signature& key);

	void add(const signature& value)
	{
		const auto index = hash_index(value);

		if (htable[index])
		{
			auto& list = *htable[index];
			auto  it = list.begin();

			while(it != list.end())
			{
				if (it->first == value)
				{
					++it->second;
					return;
				}
				else if (value < it->first)
				{
					break;
				}

				++it;
			}

			htable[index]->emplace(it, value, 1);
		}
		else
		{
			htable[index] = std::unique_ptr<bucket>(new bucket());
			htable[index]->emplace_back(value, 1);
		}
	}

	size_t not_empty_buckets()
	{
		size_t count = 0;

		for (const auto& bucket : htable)
		{
			if (bucket)
			{
				count += bucket->size();
			}
		}

		return count;
	}
};

size_t signature_table::hash_index(const signature& key)
{
	constexpr size_t value = 1000;

	float aa, bb, cc;

	std::tie(aa, bb, cc) = key.sign;

	const auto hash = ((static_cast<size_t>(bb + cc * value) % kGRandMax) + kGRandMax) % kGRandMax;
	const auto index = hash % size;

	return index;
}

template<>
signature::triplet signature::process(
                                      sign_key_type aa,
                                      sign_key_type bb,
                                      sign_key_type cc)
{
	if (aa > bb) { std::swap(aa, bb); }
	if (bb > cc) { std::swap(bb, cc); }
	if (aa > bb) { std::swap(aa, bb); }

	const float normalize = std::max(1.f, static_cast<float>(aa)); // static_cast<float>(aa); //

	return signature::triplet(
		static_cast<float>(aa) / normalize,
		static_cast<float>(bb) / normalize,
		static_cast<float>(cc) / normalize
	);
}


void W3T5::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("3\n6 6 10 15 25 15 35 21 21").expect("1");
		reference->open(*this).input("4\n3 4 5 10 11 12 6 7 8 6 8 10 ").expect("3");

		reference->open(*this).input("4\n1 1 1 1 1 1 1 1 1 1 1 1 ").expect("1");

		reference->open(*this).input("4\n1 1 3  1 3 1  3 1 1  1 1 1 ").expect("2");


		reference->open(*this).input("2\n 1 2 3 100000000 200000000 300000000 ").expect("1");

		reference->open(*this).input("3\n 1 2 3 0 1 2 0 1 2").expect("2");

		size_t count = 1'000'000;

		std::string input = std::to_string(count) + ' ';

		for (size_t i = 1; i <= count; ++i)
		{
			const auto ch = std::to_string(i);

			input += ch + ' ' + ch + ' ' + ch + ' ';
		}

		reference->open(*this).input(input).expect("1");
	}
}

void W3T5::main(std::istream& input, std::ostream& output)
{
	size_t length;
	sign_key_type la, lb, lc;
	signature_table table;

	input >> length;

	for (size_t index = 0; index < length; ++index)
	{
		input >> la >> lb >> lc;

		signature key(la, lb, lc);

		table.add(key);
	}

	output << table.not_empty_buckets();
}