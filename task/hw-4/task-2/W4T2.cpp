#include "W4T2.hpp"
#include <map>
#include <iterator>
#include <vector>
#include <algorithm>
#include <memory>

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
struct map
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

	inline size_t size() const
	{
		return pttree.size();
	}
};

void W4T2::main(std::istream& input, std::ostream& output)
{
	int x1, y1, x2, y2;

	map<int, Rectangle<int>::Point> pttree;
	std::vector<Rectangle<int>::Point> ptstorage;

	size_t count;
	input >> count;

	ptstorage.reserve(count * 3U);

	for (size_t idx = 0; idx < count; ++idx)
	{
		input >> x1 >> y1 >> x2 >> y2;

		emplace(ptstorage, x1, y1, x2, y2);
	}

	std::sort(ptstorage.begin(), ptstorage.end());

	size_t outer_count = 0;
	
	auto it_point = ptstorage.begin();

	while(it_point != ptstorage.end())
	{
		const auto it_pair = pttree.lower_bound(it_point->x); // ~ log(n)

		// существует парная точка
		if ((it_pair != pttree.end()) && (it_pair->second.x == it_point->x))
		{
			// если парная точка - левая, значит это верхний левый угол "внешнего" прямоугольника
			outer_count += static_cast<int>((it_pair->second.features & Rectangle<int>::Point::left) > 0);

			// уничтожаем парную точку
			pttree.erase(it_pair);
		}
		else
		{
			// для верхней точки не существует пары в дереве, значит парная точка была удалена
			if (it_point->features & Rectangle<int>::Point::top)
			{
				std::advance(it_point, 1);
				continue;
			}

			if (pttree.size() > 0)
			{
				//const auto it_new = pttree.emplace_hint(it_pair, point.x, point);
				//const auto prev = std::prev(it_new);

				const auto prev = std::prev(it_pair);

				// если новая точка левая и предыдущая точка в дереве тоже левая
				if (it_point->features & prev->second.features & Rectangle<int>::Point::left)
				{
					// значит эта точка принадлежит "внутреннему" прямоугольнику (мы находимся в еще бОльшем прямоугольнике)
					
					// уничтожаем ее
					//pttree.erase(it_new);

					// дополнительно пропускаем следующую за ней точку
					std::advance(it_point, 2);
					continue;
				}
			}

			//pttree.insert(it_pair, *it_point);

			// иначе нужно учитывать эту точку
			pttree.emplace_hint(it_pair, it_point->x, *it_point);
		}

		std::advance(it_point, 1);
	}

	output << outer_count << '\n';
}

void W4T2::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("3\n-3 -3 3 3\n-2 2 2 -2\n-1 -1 1 1\n").expect("1\n");
		reference->open(*this).input("4\n0 0 3 3\n1 1 2 2\n100 100 101 101\n200 200 201 201\n").expect("3\n");

		const int inner_size = 1000;
		const int outer_size = 100;
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