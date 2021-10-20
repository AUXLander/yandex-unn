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



struct Vertex;
struct PlaceFixedList;

struct Node
{
    PlaceFixedList& list;

    Vertex* value;

    bool isVisible;

    Node* pPrev;
    Node* pNext;

    Node(PlaceFixedList& list, Vertex* value, const bool isVisible = false, Node* prev = nullptr)
        : list(list), value(value), isVisible(isVisible), pPrev(prev), pNext(nullptr)
    {
        if (prev != nullptr)
        {
            prev->pNext = this;
        }
    }

    void hide();
    void show();
};

struct Vertex
{
    int xx;
    int yy;

    Node* pNode;

    explicit Vertex(const int xx, const int yy, const bool visit = false, Node* pNode = nullptr);

    inline void setNode(Node* pNode);

    inline static int distance(const Vertex& first, const Vertex& second);

    inline void visit();

    inline void unvisit();

    inline Vertex& operator=(const Vertex& other);

    inline bool operator<(const Vertex& other) const;
};

class PlaceFixedList
{
    friend class Node;
    friend class Vertex;

    const size_t cap;
    size_t sz;

    char* pAllocationB;

    Node* pAllocation;
    Node* pFirst;
    Node* pLast;

public:
    explicit PlaceFixedList(std::vector<Vertex>& container)
        : cap(container.size()), sz(0),
        pAllocationB(nullptr), pAllocation(nullptr), pFirst(nullptr), pLast(nullptr)
    {
        pAllocationB = new char[sizeof(Node) * cap];

        pAllocation = reinterpret_cast<Node*>(pAllocationB);

        size_t index = 0;

        Node* pPrevious = nullptr;

        if (cap > 0)
        {
            pFirst = pPrevious = new(pAllocation) Node(*this, 
                container.data() + index, false, nullptr);

            pPrevious->value->pNode = pPrevious;

            for (index = 1; index < cap; ++index)
            {
                pPrevious = new(pAllocation + index) Node(*this, 
                    container.data() + index, false, pPrevious);

                pPrevious->value->pNode = pPrevious;
            }

            if (cap > 0)
            {
                pLast = pPrevious;
            }
        }

        sz = index;
    }

    inline Node* first()
    {
        return pFirst;
    }

    inline Node* last()
    {
        return pLast;
    }

    inline size_t size() const
    {
        return sz;
    }

    inline size_t capacity() const
    {
        return cap;
    }

    ~PlaceFixedList()
    {
        delete[] pAllocationB;
    }
};

class VertexBuffer
{
    size_t locked_count = 0;

    PlaceFixedList cache;

public:
    explicit VertexBuffer(std::vector<Vertex>& map) : cache(map) {}

    void lock(Vertex& element);
    void unlock(Vertex& element);

    Vertex* next(const size_t offset);

    size_t avaible() const
    {
        return cache.capacity() - locked_count;
    }
};

class W2T5 : public Task
{
    void test(Test* const reference) override final {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    W2T5() : Task(nullptr) {}
    explicit W2T5(Test* const reference) : Task(reference) {}
};

Vertex::Vertex(const int xx, const int yy, const bool visit, Node* pNode)
    : xx(xx), yy(yy), pNode(pNode)
{}

void Vertex::setNode(Node* pNode)
{
    this->pNode = pNode;
}

int Vertex::distance(const Vertex& first, const Vertex& second)
{
    return std::abs(first.xx - second.xx);
}

void Vertex::visit()
{
    if (pNode != nullptr)
    {
        pNode->hide();
    }
}

void Vertex::unvisit()
{
    if (pNode != nullptr)
    {
        pNode->show();
    }
}

Vertex& Vertex::operator=(const Vertex& other)
{
    xx = other.xx;
    yy = other.yy;

    return *this;
}

bool Vertex::operator<(const Vertex& other) const
{
    return this->yy < other.yy;
}




void Node::hide()
{
    isVisible = false;

    if (pPrev != nullptr)
    {
        pPrev->pNext = pNext;
    }
    else
    {
        list.pFirst = pNext;
    }

    if (pNext != nullptr)
    {
        pNext->pPrev = pPrev;
    }
    else
    {
        list.pLast = pPrev;
    }

    --list.sz;
}

void Node::show()
{
    isVisible = true;

    if (pPrev != nullptr)
    {
        pPrev->pNext = this;
    }
    else
    {
        list.pFirst = this;
    }

    if (pNext != nullptr)
    {
        pNext->pPrev = this;
    }
    else
    {
        list.pLast = this;
    }

    ++list.sz;
}




void VertexBuffer::lock(Vertex& element)
{
    element.visit();

    ++locked_count;
}

void VertexBuffer::unlock(Vertex& element)
{
    element.unvisit();

    --locked_count;
}

Vertex* VertexBuffer::next(const size_t offset)
{
    size_t sub_offset = 0;

    auto* pNode = cache.first();

    if (pNode != nullptr)
    {
        for (; sub_offset < offset; ++sub_offset)
        {
            pNode = pNode->pNext;
        }

        return pNode->value;
    }

    return nullptr;
}

constexpr int invalid_time_value = 10'000'000;

int research(VertexBuffer& container, Vertex& element, const int time)
{
    container.lock(element);

    int min_time = invalid_time_value;
    const size_t avaible_count = container.avaible();

    Vertex* pNext;

    for (size_t offset = 0; offset < avaible_count; ++offset)
    {
        pNext = container.next(offset);

        if (pNext != nullptr)
        {
            Vertex& next = *pNext;

            const int distance = Vertex::distance(next, element);

            if (next.yy - distance - time >= 0)
            {
                min_time = std::min(min_time, research(container, next, time + distance));
            }
            else
            {
                container.unlock(element);

                return invalid_time_value;
            }
        }
        else
        {
            container.unlock(element);

            return time;
        }
    }

    container.unlock(element);

    if (avaible_count == 0)
    {
        return time;
    }

    return min_time;
}

void W2T5::main(std::istream& input, std::ostream& output)
{
    std::vector<Vertex> map;

    int SIZE;
    int xx, yy;

    input >> SIZE;

    map.reserve(SIZE);

    for (int i = 0; i < SIZE; ++i)
    {
        input >> xx;
        input >> yy;

        map.emplace_back(xx, yy);
    }

    if (SIZE == 1)
    {
        output << 0;

        return;
    }

    std::sort(map.begin(), map.end());

    VertexBuffer container(map);

    int min_time = invalid_time_value;

    for (auto& vertex : map)
    {
        min_time = std::min(min_time, research(container, vertex, 0));
    }

    if (min_time == invalid_time_value)
    {
        output << "No solution";
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

    W2T5 task;

    task.run(std::cin, std::cout);

    return 0;
}


