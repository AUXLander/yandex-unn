#include "W3T5.h"
#include <tuple>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>

template<typename T, typename H>
struct signature_t
{
	using triplet = std::tuple<H, H, H>;

	const triplet sign;

	signature_t(const T a, const T b, const T c)
		: sign(this->hash(a, b, c))
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

	triplet hash(T a, T b, T c) 
	{ 
		return triplet(); 
	};
};

using sign_key_type = unsigned int;
using signature = signature_t<sign_key_type, float>;

using sign_word = std::unique_ptr<signature>;
using sign_dictionary = std::vector<sign_word>;

signature::triplet signature::hash(sign_key_type a,
								   sign_key_type b, 
	                               sign_key_type c)
{
	if (a > b) { std::swap(a, b); }
	if (b > c) { std::swap(b, c); }
	if (a > b) { std::swap(a, b); }

	const float normalize = a;

	return signature::triplet(
		static_cast<float>(a) / normalize,
		static_cast<float>(b) / normalize,
		static_cast<float>(c) / normalize
	);
}

inline bool operator<(const sign_word& lhs, const sign_word& rhs)
{
	return (*lhs) < (*rhs);
}

inline bool operator==(const sign_word& lhs, const sign_word& rhs)
{
	return (*lhs) == (*rhs);
}

sign_dictionary::iterator binary_insert(sign_dictionary& dict, const sign_word& word)
{
	if (dict.size() > 0)
	{
		auto first = dict.begin();
		auto last = dict.end() - 1;

		while (first < last)
		{
			auto mid = first + (last - first) / 2;

			if (word == *mid)
			{
				auto copy = mid;
				while ((mid < dict.end()) && (word == *mid))
				{
					copy = mid++;
				}

				return copy;
			}
			else if (word < *mid)
			{
				last = mid;
			}
			else
			{
				first = mid + 1;
			}
		}

		return last;
	}

	return dict.end();
}

void W3T5::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("3\n6 6 10 15 25 15 35 21 21").expect("1");
		reference->open(*this).input("4\n3 4 5 10 11 12 6 7 8 6 8 10 ").expect("3");

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
	sign_dictionary dictionary;

	input >> length;

	dictionary.reserve(length);

	for (size_t index = 0, offset = 0; index < length; ++index)
	{
		input >> la >> lb >> lc;

		std::unique_ptr<signature> snext(new signature(la, lb, lc));

		const auto it = binary_insert(dictionary, snext);

		if (it != dictionary.end())
		{
			if (!(*it == snext))
			{
				dictionary.insert(it, std::move(snext));
			}
		}
		else
		{
			dictionary.push_back(std::move(snext));
		}
	}

	output << dictionary.size();
}