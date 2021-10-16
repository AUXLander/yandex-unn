#include <iterator>
#include <iostream>

#include <cstddef>
#include <utility>
#include <algorithm>
#include <functional>

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

class W2T2 : public Task
{
    void test(Test* const reference) override final {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    W2T2() : Task(nullptr) {}
    explicit W2T2(Test* const reference) : Task(reference) {}
};

struct AssociatedValue : private std::pair<int, int>
{
    using LambdaOperator = const std::function<bool(const AssociatedValue&, const AssociatedValue&)>;

    static LambdaOperator less_value;
    static LambdaOperator less_index;

    int& value = std::pair<int, int>::first;
    int& index = std::pair<int, int>::second;

    AssociatedValue() : std::pair<int, int>() {}
    AssociatedValue(const int& value, const int& index) : std::pair<int, int>(value, index) {}

    AssociatedValue(AssociatedValue&& other) noexcept : std::pair<int, int>(std::move(other)) {}

    explicit AssociatedValue(const AssociatedValue& other) : std::pair<int, int>(other.value, other.index) {}

    AssociatedValue& operator= (const AssociatedValue& other)
    {
        value = other.value;
        index = other.index;

        return *this;
    }

    inline bool operator== (const AssociatedValue& other) const
    {
        return value == other.value;
    }
};

AssociatedValue::LambdaOperator AssociatedValue::less_value = [](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
    return lhs.value < rhs.value;
};

AssociatedValue::LambdaOperator AssociatedValue::less_index = [](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
    return lhs.index < rhs.index;
};

static int leftBoundSearch(const AssociatedValue* pBegin, const AssociatedValue* pEnd, const int offset, const int value)
{
    const int length = static_cast<int>(pEnd - pBegin);

    int left = offset;
    int right = length - 1U;

    while (right - left > 1)
    {
        int middle = (left + right) / 2;

        if (pBegin[middle].value <= value)
        {
            left = middle;
        }
        else
        {
            right = middle;
        }
    }

    while ((left + 1 < length) && (pBegin[left + 1].value <= value))
    {
        ++left;
    }

    return left;
}

static void printIndex(std::ostream& output, const AssociatedValue* pBegin, const AssociatedValue* const pEnd)
{
    do
    {
        output << pBegin->index << ' ';
    }
    while (++pBegin != pEnd);
}

void W2T2::main(std::istream& input, std::ostream& output)
{
    std::istream_iterator<int, char> istream(input);

    constexpr int reserved_for_summ_0 = 1U;

    constexpr int reserved = reserved_for_summ_0;

    const int length = next(istream);

    if (length <= 0)
    {
        output << 0 << '\n';

        return;
    }

    std::byte* pAllocation = new std::byte[sizeof(AssociatedValue) * length + sizeof(int) * (length + reserved)];

    AssociatedValue* const pBeginValue = reinterpret_cast<AssociatedValue*>(pAllocation);
    AssociatedValue* const pEndValue = reinterpret_cast<AssociatedValue*>(pAllocation) + length;

    AssociatedValue* const pValues = pBeginValue;

    int* const pSums = reinterpret_cast<int*>(pEndValue);

    for (int index = 0; index < length; ++index)
    {
        new(pBeginValue + index) AssociatedValue(next(istream), index + 1);
    }

    std::sort(pBeginValue, pEndValue, AssociatedValue::less_value);

    pSums[0] = 0;

    for (int idx = 0; idx < length; ++idx)
    {
        pSums[reserved + idx] = pSums[idx] + pValues[idx].value;
    }

    int max = pSums[0];

    AssociatedValue* pBeginIndex = pBeginValue;
    AssociatedValue* pEndIndex = pBeginValue;

    for (int index = 0; index < length - 1; ++index)
    {
        const int ai = pValues[index + 0U].value;
        const int aj = pValues[index + 1U].value;

        const int boundIdx = leftBoundSearch(pBeginValue, pEndValue, index, ai + aj);

        const int hSum = pSums[boundIdx + 1U];
        const int lSum = pSums[index];

        if (hSum - lSum > max)
        {
            max = hSum - lSum;

            pBeginIndex = pBeginValue + index;
            pEndIndex = pBeginValue + boundIdx + 1;
        }
    }

    std::sort(pBeginIndex, pEndIndex, AssociatedValue::less_index);

    output << max << '\n';

    printIndex(output, pBeginIndex, pEndIndex);

    delete[] pAllocation;
}

int main(int, char*[])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    W2T2 task;

    task.run(std::cin, std::cout);

    return 0;
}
