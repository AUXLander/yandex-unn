// #define DEBUG_rW1T4

#ifndef DEBUG_rW1T4
#define RELEASE
#endif

#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>


class Test {};

class Task
{
protected:
    size_t m_index;

    Test* const m_reference;

    inline void reset() { m_index = 0; }

    template<typename T>
    T next(std::istream_iterator<T, char>& input)
    {
        if (m_index > 0)
        {
            ++input;
        }

        m_index++;

        if (input == std::istream_iterator<T, char>())
        {
            if constexpr (std::is_same<T, char>())
            {
                return '\0';
            }

            return static_cast<T>(NULL);
        }

        return *input;
    }

    virtual void test(Test* const reference) = 0;
    virtual void main(std::istream& input, std::ostream& output) = 0;

public:
    explicit Task(Test* const reference) : m_index(0), m_reference(reference) {};

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
        reset();
        main(input, output);
    }
};


template<class T>
struct Point
{
    using value_type = T;

    value_type x;
    value_type y;

    Point(const value_type x, const value_type y) : x(x), y(y) {}

    inline bool operator<(const Point& other) const
    {
        return (this->x < other.x) || ((this->x == other.x) && (this->y < other.y));
    }
};

struct RadiusDirection
{
    using value_type = size_t;
    enum Direction : value_type
    {
        left = 0, right = 1
    };

    // The power of Direction set;
    constexpr static size_t size{ 2U };

private:
    Direction direction;
public:
    explicit RadiusDirection(const RadiusDirection::Direction direction) : direction(direction) {}

    static inline value_type transformation(const Direction direction)
    {
        return static_cast<value_type>(direction);
    }
};

template<class T>
struct Span
{
    using value_type = T;

public:
    value_type xoffset;
    RadiusDirection::Direction direction;

    Span() 
        : xoffset(std::numeric_limits<value_type>::max()), 
          direction(RadiusDirection::Direction::left) {}

    Span(const value_type xoffset, const RadiusDirection direction) 
        : xoffset(xoffset), direction(direction) {}

    inline bool operator<(const Span<T>& other) const
    {
        return this->xoffset < other.xoffset;
    }

    inline void set(const value_type xoffset, const RadiusDirection::Direction direction)
    {
        this->xoffset = xoffset;
        this->direction = direction;
    }
};

class rW1T4 : public Task
{
    using SpanT = Span<double>;
    using PointT = Point<double>;

    static constexpr double precision = 1e-4;

    std::vector<SpanT> buffer;

    void initialize(const size_t size)
    {
        buffer.resize(size + size);
    }

    int countPointsRadius(const std::vector<PointT>& points, const double radius);

    void test(Test* const reference) override final {}
    void main(std::istream& input, std::ostream& output) override final;

public:
    rW1T4() : Task(nullptr) {}
    explicit rW1T4(Test* const reference) : Task(reference) {}
};


int rW1T4::countPointsRadius(const std::vector<PointT>& points, const double radius)
{
    auto lspanIterator = buffer.begin();
    auto rspanIterator = buffer.rbegin();

    for (const auto& point : points)
    {
        const auto xsqr = (radius * radius) - (point.y * point.y);

        const bool isComplex = (xsqr < 0.0);

        const auto loffset = isComplex ?
            std::numeric_limits<double>::max() : point.x - std::sqrt(xsqr);

        const auto roffset = isComplex ? 
            std::numeric_limits<double>::max() : point.x + std::sqrt(xsqr);

        lspanIterator->set(loffset, RadiusDirection::left);
        rspanIterator->set(roffset, RadiusDirection::right);

        std::advance(lspanIterator, 1U);
        std::advance(rspanIterator, 1U);
    }

    std::sort(buffer.begin(), buffer.end());

    constexpr static long int accumulation[RadiusDirection::size] = { +1, -1 };

    int max_accumulated = 0;
    int accumulated = 0;

    for (const auto& span : buffer)
    {
        if (span.xoffset > 2001.0)
        {
            break;
        }

        accumulated += accumulation[RadiusDirection::transformation(span.direction)];

        max_accumulated = std::max(max_accumulated, accumulated);
    }

    return max_accumulated;
}

void rW1T4::main(std::istream& input, std::ostream& output)
{
    output << std::fixed;
    output.precision(6);

    std::vector<PointT> points;

    std::istream_iterator<int, char> istream(input);

    const int size = next(istream);
    const int requested_count = next(istream);

    points.reserve(size);

    for (int index = 0; index < size; ++index)
    {
        const auto x = next(istream);
        const auto y = next(istream);

        points.emplace_back(x, y);
    }

    initialize(points.size());

    double start = 0.0;
    double end = 1415.0;
    double radius;

    const auto check = [this, &points, &requested_count](const double radius) -> bool
    {
        return countPointsRadius(points, radius) >= requested_count;
    };

    while ((end - start) > precision)
    {
        radius = (end + start) * 0.5;

        if (check(radius) == true)
        {
            end = radius;
        }
        else
        {
            start = radius;
        }
    }

    output << end << std::endl;
}

#ifndef DEBUG_rW1T4
int main(int argc, char* argv[])
{
    rW1T4 task;

    task.run(std::cin, std::cout);

    return 0;
}

#endif // !DEBUG
