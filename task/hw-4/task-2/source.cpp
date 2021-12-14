#include <map>
#include <iterator>
#include <vector>
#include <algorithm>
#include <memory>
#include <deque>
#include <iostream>

class Test {};

class Task
{
protected:
    Test* const m_reference;

    virtual void test(Test* const reference) = 0;
    virtual void main(std::istream& input, std::ostream& output) = 0;

public:
    explicit Task(Test* const reference) : m_reference(reference) {};

    Task(Task&&) = delete;
    Task(const Task&) = delete;

    virtual ~Task() {};

    void selftest()
    {
        if (m_reference != nullptr)
        {
            test(m_reference);
        }
    }

    void run(std::istream& input, std::ostream& output)
    {
        main(input, output);
    }
};

class W4T2 : public Task
{
    void test(Test* const reference) {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    W4T2() : Task(nullptr) {}
    explicit W4T2(Test* const reference) : Task(reference) {}
};

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


void emplace(std::vector<Rectangle<int>::Point>& storage, const int& xl, 
                const int& yl, const int& xr, const int& yr)
{
    using vside = Rectangle<int>::Point::vside;
    using hside = Rectangle<int>::Point::hside;

    storage.emplace_back(std::min(xl, xr), std::max(yl, yr), vside::top,    hside::left);
    storage.emplace_back(std::min(xl, xr), std::min(yl, yr), vside::bottom, hside::left);
    storage.emplace_back(std::max(xl, xr), std::max(yl, yr), vside::top,    hside::right);
    storage.emplace_back(std::max(xl, xr), std::min(yl, yr), vside::bottom, hside::right);
}

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

    inline void erase(iterator wh) noexcept
    {
        ptunmap.erase(wh);
    }

    inline iterator emplace_hint(iterator wh, const Tkey& key, const Tval& val)
    {
        return ptunmap.insert(wh, std::make_pair(key, val));
    }

    inline iterator emplace(const Tkey& key, const Tval& val)
    {
        ptunmap.emplace_back(std::make_pair(key, val));
        return ptunmap.begin();
    }
    
    inline size_t size() const
    {
        return ptunmap.size();
    }
};

void W4T2::main(std::istream& input, std::ostream& output)
{
    int xl, yl, xr, yr;

    map<int, Rectangle<int>::Point> pttree;

    std::vector<Rectangle<int>::Point> ptstorage;

    size_t count;
    input >> count;

    ptstorage.reserve(3U * count);

    for (size_t idx = 0; idx < count; ++idx)
    {
        input >> xl >> yl >> xr >> yr;

        emplace(ptstorage, xl, yl, xr, yr);
    }

    std::sort(ptstorage.begin(), ptstorage.end());

    size_t outer_count = 0;
    
    auto it_point = ptstorage.begin();

    pttree.emplace(it_point->x, *it_point);

    ++it_point;

    while (it_point != ptstorage.end())
    {
        auto it_pair = pttree.lower_bound(it_point->x); // ~ log(n)

        // существует парная точка
        if ((it_pair != pttree.end()) && (it_pair->second.x == it_point->x))
        {
            // если парная точка - левая, значит это верхний левый угол "внешнего" прямоугольника
            outer_count += static_cast<int>((it_pair->second.features & 
                        Rectangle<int>::Point::left) > 0);

            // уничтожаем парную точку
            pttree.erase(it_pair);
            std::advance(it_point, 1);
            continue;
        }
        else
        {
            // для верхней точки не существует пары в дереве, значит парная точка была удалена
            if (it_point->features & Rectangle<int>::Point::top)
            {
                std::advance(it_point, 1);
                continue;
            }

            if (pttree.size() > 0 && it_pair != pttree.begin())
            {
                auto prev = std::prev(it_pair);

                if (prev != pttree.end())
                {
                    // если новая точка левая и предыдущая точка в дереве тоже левая
                    if (it_point->features & prev->second.features & Rectangle<int>::Point::left)
                    {
                        // дополнительно пропускаем следующую за ней точку
                        std::advance(it_point, 2);
                        continue;
                    }
                }
            }

            // иначе нужно учитывать эту точку
            pttree.emplace_hint(it_pair, it_point->x, *it_point);
        }

        std::advance(it_point, 1);
    }

    output << outer_count << '\n';
}

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    W4T2 task;

    task.run(std::cin, std::cout);

    return 0;
}
