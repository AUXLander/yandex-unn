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

class W2T3 : public Task
{
    void test(Test* const reference) override final {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    W2T3() : Task(nullptr) {}
    explicit W2T3(Test* const reference) : Task(reference) {}
};

template<class T>
class Container
{
    std::byte* const pAllocation;
public:
    const int size;

    T* const pBegin = reinterpret_cast<T* const>(pAllocation);
    T* const pEnd = reinterpret_cast<T* const>(pAllocation) + size;

    explicit Container(const int size) 
        : pAllocation(new std::byte[sizeof(T) * size]), size(size) {;}

    inline T* data()
    {
        return pBegin;
    }

    inline T& operator[](const int index)
    {
        return *(reinterpret_cast<T*>(pAllocation) + index);
    }

    ~Container()
    {
        delete[] pAllocation;
    }
};

template<class T>
void merge(T* xPtr, const int xSize, T* yPtr, const int ySize, T* destination)
{
    T* xPtrEnd = xPtr + xSize;
    T* yPtrEnd = yPtr + ySize;

    while ((xPtr < xPtrEnd) && (yPtr < yPtrEnd))
    {
        if ((*xPtr) < (*yPtr))
        {
            *destination = *xPtr;

            ++xPtr;
        }
        else
        {
            *destination = *yPtr;

            ++yPtr;
        }

        ++destination;
    }

    std::copy(xPtr, xPtrEnd, destination);
    std::copy(yPtr, yPtrEnd, destination);
}

void W2T3::main(std::istream& input, std::ostream& output)
{
    std::istream_iterator<int, char> istream(input);

    const int size = next(istream);
    const int length = next(istream);

    const int total = size * length;

    if (size == 1)
    {
        for (int elementIdx = 0; elementIdx < size * length; ++elementIdx)
        {
            output << next(istream) << ' ';
        }

        return;
    }

    // ===============================
    // |    swap     |     main      |
    // ===============================

    Container<int> container(total + total);

    int* pMemory = container.data();
    int* pSwap = container.data() + total;

    for (int index = 0; index < total; ++index)
    {
        pSwap[index] = pMemory[index] = next(istream);
    }

    int offset = 0;
    int subhl = length;
    int subsz = subhl + subhl;

    while (subhl <= total)
    {
        offset = 0;

        while (offset + subsz <= total)
        {
            const auto lsz = std::min(total, subhl);
            const auto rsz = std::min(total - lsz, subhl);

            int* const pLeft = pMemory + offset;
            int* const pRight = pMemory + offset + lsz;

            merge(pLeft, lsz, pRight, rsz, pSwap + offset);

            offset += lsz + rsz;
        }

        if (offset + subhl < total)
        {
            const auto lsz = subhl;
            const auto rsz = total - offset - lsz;

            int* const pLeft = pMemory + offset;
            int* const pRight = pMemory + offset + lsz;

            merge(pLeft, lsz, pRight, rsz, pSwap + offset);
        }
        else if (offset < total)
        {
            std::copy(pMemory + offset, pMemory + total, pSwap + offset);
        }

        subhl = subsz;
        subsz += subsz;

        std::swap(pSwap, pMemory);
    }

    for (int index = 0; index < total; ++index)
    {
        output << pMemory[index] << ' ';
    }
}

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    W2T3 task;

    task.run(std::cin, std::cout);

    return 0;
}
