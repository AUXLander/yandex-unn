#include <algorithm>
#include <iterator>
#include <iostream>
#include <utility>
#include <vector>
#include <cmath>

struct Point : private std::pair<double, double>
{
    double& coord_x = std::pair<double, double>::first;
    double& coord_y = std::pair<double, double>::second;

    Point() : std::pair<double, double>() {}
    Point(const double coord_x, const double coord_y) : std::pair<double, double>(coord_x, coord_y) {}

    inline bool operator<(const Point& other) const
    {
        return (this->coord_x < other.coord_x) || ((this->coord_x == other.coord_x) && (this->coord_y < other.coord_y));
    }
};

using BoundType = bool;

struct Bound : private std::pair<double, BoundType>
{
    double& xoffset = std::pair<double, BoundType>::first;
    BoundType& type = std::pair<double, BoundType>::second;

    Bound() : std::pair<double, BoundType>() {}
    Bound(const double xoffset, const BoundType type)
        : std::pair<double, BoundType>(xoffset, type) {}

    Bound(Bound&& other) noexcept : std::pair<double, BoundType>(std::move(other)) {}

    inline bool operator<(const Bound& other) const
    {
        return this->xoffset < other.xoffset;
    }

    inline Bound& operator=(const Bound& other)
    {
        xoffset = other.xoffset;
        type = other.type;

        return *this;
    }
};


void processPoints(const std::vector<Point>& points, 
                   std::vector<Bound>& buffer, const double radius)
{
    size_t lpos = 0;
    size_t rpos = buffer.size() - 1;

    for (const auto& point : points)
    {
        const double cxx = std::pow(radius, 2.0) - std::pow(point.coord_y, 2.0);

        if (cxx >= 0.0)
        {
            const double cx = std::pow(cxx, 0.5);

            buffer[lpos].xoffset = point.coord_x - cx;
            buffer[rpos].xoffset = point.coord_x + cx;
        }
        else
        {
            buffer[lpos].xoffset = 150000.0;
            buffer[rpos].xoffset = 150000.0;
        }

        buffer[lpos].type = true;
        buffer[rpos].type = false;

        lpos += 1;
        rpos -= 1;
    }

    std::sort(buffer.begin(), buffer.end());
}

int countPointsRadius(const std::vector<Point>& points, 
                      const std::vector<Bound>& buffer, const double radius)
{
    int max = 0;
    int timesIn = 0;
    int timesOut = 0;

    for (const auto& item : buffer)
    {
        if (item.xoffset > 2000.0)
        {
            break;
        }

        timesIn += static_cast<int>(item.type == true);
        timesOut += static_cast<int>(item.type == false);

        max = std::max(max, timesIn - timesOut);
    }

    return max;
}

int main(int, char* [])
{
    int size;
    int tx, ty;
    int requested_count;

    std::cin >> size;
    std::cin >> requested_count;

    std::vector<Point> points;
    std::vector<Bound> buffer;

    buffer.resize(2U * size);
    points.resize(1U * size);

    for (int index = 0; index < size; ++index)
    {
        std::cin >> tx;
        std::cin >> ty;

        points[index].coord_x = tx;
        points[index].coord_y = ty;
    }

    double start = 0.0;
    double end = 2000.0;

    while (end - start > 0.0001)
    {
        double radius = (end + start) / 2U;

        processPoints(points, buffer, radius);

        const int current_count = countPointsRadius(points, buffer, radius);

        if (current_count >= requested_count)
        {
            end = radius;
        }
        else
        {
            start = radius;
        }
    }

    std::cout.precision(6);
    std::cout << std::fixed << end << '\n';

    return 0;
}
