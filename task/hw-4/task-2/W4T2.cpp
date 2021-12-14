#include "W4T2.hpp"
#include <map>
#include <iterator>
#include <vector>
#include <algorithm>
#include <memory>
#include <deque>
#include <list>

#include "mytree.h"

//#include "avl-tree.h"

//#include "map.h"


template<class T>
struct Point
{
	T x;
	T y;

	uint16_t features;

	Point() = default;

	explicit Point(const Point<T>& other) noexcept
		: x(other.x), y(other.y), features(other.features)
	{;}

	Point(const T& x, const T& y) noexcept
		: x(x), y(y), features(0)
	{;}

	Point(const T& x, const T& y, const uint16_t features) noexcept
		: x(x), y(y), features(features)
	{;}

	inline Point<T>& operator=(const Point<T>& other)
	{
		x = other.x;
		y = other.y;
		features = other.features;

		return *this;
	}
};

template<class T>
struct Rectangle
{
	struct Point : public ::Point<T>
	{
		enum vside : uint16_t
		{
			top = 1U << 0U, 
			bottom = 1U << 1U
		};

		enum hside : uint16_t
		{
			left = 1U << 2U,
			right = 1U << 3U
		};

		Point() = default;

		Point(Point&&) = default;

		Point(const T x, const T y, const vside vs, const hside hs) noexcept
			: ::Point<T>(x, y, vs | hs)
		{;}

		explicit Point(const Point& other) noexcept
			: ::Point<T>(other)
		{;}

		inline bool operator< (const Point& other) const
		{
			return (this->y < other.y) || ((this->y == other.y) && (this->x < other.x));
		}

		inline Point& operator= (const Point& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->features = other.features;

			return *this;
		}
	};
};

void emplace(std::vector<Rectangle<int>::Point>& storage, const int& x1, const int& y1, const int& x2, const int& y2)
{
	using vside = Rectangle<int>::Point::vside;
	using hside = Rectangle<int>::Point::hside;

	storage.emplace_back(std::min(x1, x2), std::max(y1, y2), vside::top,	hside::left);
	storage.emplace_back(std::min(x1, x2), std::min(y1, y2), vside::bottom, hside::left);
//	storage.emplace_back(std::max(x1, x2), std::max(y1, y2), vside::top,	hside::right);
	storage.emplace_back(std::max(x1, x2), std::min(y1, y2), vside::bottom, hside::right);
}

template<class Tkey, class Tval>
struct origin_map
{
	using zmap = std::map<Tkey, Tval>;
	using iterator = typename zmap::iterator;

	zmap pttree;

	inline iterator begin() noexcept
	{
		return pttree.begin();
	}

	inline iterator end() noexcept
	{
		return pttree.end();
	}

	inline iterator lower_bound(const Tkey& key)
	{
		// log(n)
		return pttree.lower_bound(key);
	}

	inline iterator erase(iterator wh) noexcept
	{
		return pttree.erase(wh);
	}

	inline iterator emplace_hint(iterator wh, const Tkey& key, const Tval& val)
	{
		return pttree.emplace_hint(wh, key, val);
	}

	inline iterator emplace(const Tkey& key, const Tval& val)
	{
		return pttree.emplace(key, val).first;
	}

	inline size_t size() const
	{
		return pttree.size();
	}
};


//template<class Tkey, class Tval>
//struct btree_map
//{
//	using iterator = typename btree::map<Tkey, Tval>::iterator;
//
//	btree::map<Tkey, Tval> pttree;
//
//	inline iterator begin() noexcept
//	{
//		return pttree.begin();
//	}
//
//	inline iterator end() noexcept
//	{
//		return pttree.end();
//	}
//
//	inline iterator lower_bound(const Tkey& key)
//	{
//		return pttree.lower_bound(key);
//	}
//
//	inline iterator erase(iterator wh) noexcept
//	{
//		return pttree.erase(wh);
//	}
//
//	inline iterator emplace_hint(iterator wh, const Tkey& key, const Tval& val)
//	{
//		return pttree.emplace_hint(wh, key, val);
//	}
//
//	inline iterator emplace(const Tkey& key, const Tval& val)
//	{
//		return pttree.emplace(key, val).first;
//	}
//
//	inline size_t size() const
//	{
//		return pttree.size();
//	}
//};


template<class Tkey, class Tval>
struct map
{
	using iterator = typename std::deque<std::pair<Tkey, Tval>>::iterator;

	struct less
	{
		static inline bool cmp(int rkey, std::pair<Tkey, Tval> lkey)
		{
			return lkey.first < rkey;
		}

		inline bool operator() (std::pair<Tkey, Tval> lkey, int rkey) const
		{
			return cmp(rkey, lkey);
		}
	};

	std::deque<std::pair<Tkey, Tval>> ptunmap;

	inline iterator begin() noexcept
	{
		return ptunmap.begin();
	}

	inline iterator end() noexcept
	{
		return ptunmap.end();
	}

	inline iterator lower_bound(const Tkey& key)
	{
		return std::lower_bound(ptunmap.begin(), ptunmap.end(), key, less());
	}

	inline iterator erase(iterator wh) noexcept
	{
		return ptunmap.erase(wh);
	}

	inline iterator emplace_hint(iterator wh, const Tkey& key, const Tval& val)
	{
		return ptunmap.insert(wh, std::make_pair(key, val));
	}

	inline iterator emplace(const Tkey& key, const Tval& val)
	{
		ptunmap.emplace_back(std::make_pair(key, val));
		return ptunmap.begin();

		//auto first = lower_bound(key);

		//if ((first == ptunmap.begin()) && (ptunmap.begin() == ptunmap.end()))
		//{
		//	

		//	return ptunmap.begin();
		//}

		//if ((!(first == ptunmap.end()) && !(less::cmp(key, *first))))
		//{
		//	return first;
		//}
		//else
		//{
		//	ptunmap.insert(first, std::make_pair(key, val));
		//}

		//

		//std::sort(ptunmap.begin(), ptunmap.end());

		//

		//if ((!(first == ptunmap.end()) && !(less::cmp(key,*first))))
		//{
		//	return first;
		//}

		//return ptunmap.end();
	}

	inline size_t size() const
	{
		return ptunmap.size();
	}
};


void W4T2::main(std::istream& input, std::ostream& output)
{
	int x1, y1, x2, y2;

	map<int, Rectangle<int>::Point> pttree;
	std::vector<Rectangle<int>::Point> ptstorage;

	size_t count;
	input >> count;

	ptstorage.reserve(3U * count);

	for (size_t idx = 0; idx < count; ++idx)
	{
		input >> x1 >> y1 >> x2 >> y2;

		emplace(ptstorage, x1, y1, x2, y2);
	}

	std::sort(ptstorage.begin(), ptstorage.end());

	size_t outer_count = 0;
	
	auto it_point = ptstorage.begin();

	pttree.emplace(it_point->x, *it_point);

	++it_point;

	while(it_point != ptstorage.end())
	{
		const auto it_pair = pttree.lower_bound(it_point->x); // ~ log(n)

		// ���������� ������ �����
		if ((it_pair != pttree.end()) && (it_pair->second.x == it_point->x))
		{
			// ���� ������ ����� - �����, ������ ��� ������� ����� ���� "��������" ��������������
			outer_count += static_cast<int>((it_pair->second.features & Rectangle<int>::Point::left) > 0);

			// ���������� ������ �����
			pttree.erase(it_pair);
			continue;
		}
		else
		{
			// ��� ������� ����� �� ���������� ���� � ������, ������ ������ ����� ���� �������
			if (it_point->features & Rectangle<int>::Point::top)
			{
				std::advance(it_point, 1);
				continue;
			}

			const auto prev = std::prev(it_pair);

			// ���� ����� ����� ����� � ���������� ����� � ������ ���� �����
			if (it_point->features & prev->second.features & Rectangle<int>::Point::left)
			{
				// ������ ��� ����� ����������� "�����������" �������������� (�� ��������� � ��� ������� ��������������)

				// ������������� ���������� ��������� �� ��� �����
				std::advance(it_point, 2);
				continue;
			}

			// ����� ����� ��������� ��� �����
			pttree.emplace_hint(it_pair, it_point->x, *it_point);
		}

		std::advance(it_point, 1);
	}

	output << outer_count << '\n';
}

void W4T2::test(Test* const reference)
{
	binary::tree<int, int> tr(7, 7);

	tr.emplace(3, 3);
	tr.emplace(2, 2);
	tr.emplace(6, 6);
	tr.emplace(5, 5);
	tr.emplace(10, 10);
	tr.emplace(8, 8);
	tr.emplace(9, 9);
	tr.emplace(12, 12);

	auto [node, side] = tr.explore(9);

	//if (side == binary::side::parent)
	//{
	//	tr.erase(6);
	//}

	auto rs1 = tr.lower_bound(4);
	auto rs2 = tr.lower_bound(8);
	auto rs3 = tr.lower_bound(9);
	auto rs4 = tr.lower_bound(11);

	auto res = tr.explore(4);

	return;


	if (reference != nullptr)
	{
		reference->open(*this).input("3\n-3 -3 3 3\n-2 2 2 -2\n-1 -1 1 1\n").expect("1\n");
		reference->open(*this).input("4\n0 0 3 3\n1 1 2 2\n100 100 101 101\n200 200 201 201\n").expect("3\n");

		const int inner_size = 10;
		const int outer_size = 10000;
		const int static_offset = 10 * inner_size;
		const int reserved = static_offset + inner_size;
		
		std::string innercubes(std::to_string(inner_size * outer_size) + '\n');

		for (int offset = 0; offset < outer_size * reserved; offset += reserved)
		{
			for (int pos = 0; pos < inner_size; ++pos)
			{
				int tpos = 1 + pos;

				innercubes += std::to_string(-tpos + offset) + ' ';
				innercubes += std::to_string(-tpos) + ' ';

				innercubes += std::to_string(+tpos + offset) + ' ';
				innercubes += std::to_string(+tpos) + '\n';
			}
		}

		reference->open(*this).input(innercubes).expect(std::to_string(outer_size) + '\n');
	}
}