#include <iterator>
#include <iostream>

#include <fstream>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <functional>
#include <vector>
#include <limits>

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
            else
            {
                return static_cast<T>(NULL);
            }
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

class rW2T5 : public Task
{
    void test(Test* const reference) override final {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    rW2T5() : Task(nullptr) {}
    explicit rW2T5(Test* const reference) : Task(reference) {}
};

struct Vertex;

template<class T>
class PlaceFixedList
{
    friend class Vertex;

    struct Node;

    const size_t cap;
    size_t sz;

    std::byte* pAllocationB;

    Node* pAllocation;
    Node* pFirst;
    Node* pLast;

    struct Node
    {
        PlaceFixedList& list;

        T* value;

        bool isVisible;

        Node* prev;
        Node* next;

        Node(PlaceFixedList& list, T* value, const bool isVisible = false, Node* prev = nullptr)
            : list(list), value(value), isVisible(isVisible), prev(prev), next(nullptr)
        {
            if (prev != nullptr)
            {
                prev->next = this;
            }
        }

        ~Node() {}

        void hide()
        {
            isVisible = false;

            if (prev != nullptr)
            {
                prev->next = next;
            }
            else
            {
                list.pFirst = next;
            }

            if (next != nullptr)
            {
                next->prev = prev;
            }
            else
            {
                list.pLast = prev;
            }

            --list.sz;
        }

        void show()
        {
            isVisible = true;

            if (prev != nullptr)
            {
                prev->next = this;
            }
            else
            {
                list.pFirst = this;
            }

            if (next != nullptr)
            {
                next->prev = this;
            }
            else
            {
                list.pLast = this;
            }

            ++list.sz;
        }
    };

public:
    explicit PlaceFixedList(std::vector<T>& container)
        : cap(container.size()), sz(0),
        pAllocation(nullptr), pFirst(nullptr), pLast(nullptr)
    {
        pAllocationB = new std::byte[sizeof(Node) * cap];

        pAllocation = reinterpret_cast<Node*>(pAllocationB);

        size_t index = 0;

        Node* pPrevious = nullptr;

        if (cap > 0)
        {
            pFirst = pPrevious 
                    = new(pAllocation)
                        Node(*this, container.data() + index, false, nullptr);

            for (index = 1; index < cap; ++index)
            {
                pPrevious
                    = new(pAllocation + index) 
                        Node(*this, container.data() + index, false, pPrevious);
            }

            if (cap > 0)
            {
                pLast = pPrevious;
            }
        }

        sz = index;
    }

    Node* first()
    {
        return pFirst;
    }

    Node* last()
    {
        return pLast;
    }

    size_t size() const
    {
        return sz;
    }

    size_t capacity() const
    {
        return cap;
    }

    ~PlaceFixedList()
    {
        delete[] pAllocationB;
    }
};

struct Point
{
    int xx;
    int yy;

    Point(int xx, int yy) : xx(xx), yy(yy) {};
};


struct Vertex
{
    Point point;

    bool isVisited;

    PlaceFixedList<Vertex>::Node* pNode;

    explicit Vertex(const Point& point, const bool visit = false,
            PlaceFixedList<Vertex>::Node* pNode = nullptr)
        : point(point), isVisited(visit), pNode(pNode) {}

    void reset()
    {
        isVisited = false;
    }

    void setNode(PlaceFixedList<Vertex>::Node* pNode)
    {
        this->pNode = pNode;
    }

    static int distance(const Vertex& first, const Vertex& second)
    {
        return std::abs(first.point.xx - second.point.xx);
    }

    void visit()
    {
        if (pNode != nullptr)
        {
            pNode->hide();
        }

        isVisited = true;
    }

    void unvisit()
    {
        if (pNode != nullptr)
        {
            pNode->show();
        }

        isVisited = false;
    }

    Vertex& operator=(const Vertex& other)
    {
        point = other.point;
        isVisited = other.isVisited;

        return *this;
    }
};

class VertexBuffer
{
    size_t index = 0;
    size_t locked_count = 0;

    PlaceFixedList<Vertex> cache;

public:
    explicit VertexBuffer(std::vector<Vertex>& map) : cache(map)
    {
        auto* pNode = cache.first();

        if (pNode != nullptr)
        {
            do
            {
                pNode->value->setNode(pNode);
                pNode = pNode->next;
            } while ((pNode != nullptr) && (pNode->next != nullptr));

            pNode->value->setNode(pNode);
        }

        pNode = pNode;
    }

    ~VertexBuffer() {}

    void lock(Vertex& element)
    {
        element.visit();

        ++locked_count;
    }

    void unlock(Vertex& element)
    {
        element.unvisit();

        --locked_count;
    }

    Vertex* next(const size_t offset)
    {
        index = 0;
        size_t sub_offset = 0;

        auto* pNode = cache.first();

        if (pNode != nullptr)
        {
            for (; (sub_offset < offset); ++sub_offset)
            {
                if (pNode->next == nullptr)
                {
                    if (pNode == pNode->list.last())
                    {
                        pNode = pNode->list.first();
                    }
                    else
                    {
                        return nullptr;
                    }
                }
                else
                {
                    pNode = pNode->next;
                }
            }

            return pNode->value;
        }

        return nullptr;
    }

    size_t avaible() const
    {
        return cache.capacity() - locked_count;
    }
};

constexpr int invalid_time_value = std::numeric_limits<int>::max();

int research(VertexBuffer& container, Vertex& element, const int time)
{
    container.lock(element);

    int min_time = invalid_time_value;
    const size_t avaible_count = container.avaible();

    for (size_t offset = 0; offset < avaible_count; ++offset)
    {
        auto* pNext = container.next(offset);

        if (pNext != nullptr)
        {
            auto& next = *pNext;

            auto distance = Vertex::distance(next, element);

            if (next.point.yy - distance - time >= 0)
            {
                const int new_time = research(container, next, time + distance);

                if (new_time >= 0)
                {
                    min_time = std::min(min_time, new_time);
                }
            }
            else
            {
                container.unlock(element);

                return invalid_time_value;
            }
        }
        else
        {
            min_time = time;

            break;
        }
    }

    if (avaible_count == 0)
    {
        min_time = time;
    }

    container.unlock(element);

    return min_time;
}

void rW2T5::main(std::istream& input, std::ostream& output)
{
    int NN;// = next(istream);

    input >> NN;

    std::vector<Point> points;

    points.reserve(NN);

    for (int idx = 0; idx < NN; ++idx)
    {
        int xx, yy;

        input >> xx >> yy;

        points.emplace_back(xx, yy);
    }

    if (NN == 1)
    {
        output << 0;

        return;
    }

    std::vector<Vertex> map;

    map.reserve(points.size());

    for (const auto& point : points)
    {
        map.emplace_back(point);
    }

    std::sort(map.begin(), map.end(), 
        [](const Vertex& lhs, const Vertex& rhs) { return lhs.point.yy < rhs.point.yy; });

    VertexBuffer container(map);

    int min_time = invalid_time_value;

    for (auto& vertex : map)
    {
        int time = research(container, vertex, 0);

        if (time >= 0)
        {
            min_time = std::min(time, min_time);
        }
    }

    if (min_time == invalid_time_value)
    {
        output << "No solution\n";
    }
    else
    {
        output << min_time;
    }
}

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    rW2T5 task;

    task.run(std::cin, std::cout);

    return 0;
}
