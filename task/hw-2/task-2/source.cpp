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

    inline void Reset() { m_index = 0; }

    template<typename T>
    T Next(std::istream_iterator<T, char>& input)
    {
        if (m_index > 0)
        {
            ++input;
        }

        m_index++;

        if (input == std::istream_iterator<T, char>())
        {
            return '\0';
        }

        return *input;
    }

    virtual void Main(std::istream& input, std::ostream& output) = 0;

public:
    explicit Task(Test* const reference) : m_index(0), m_reference(reference) {};

    Task(Task&&) = delete;
    Task(const Task&) = delete;

    virtual ~Task() {};

    void Run(std::istream& input, std::ostream& output)
    {
        Reset();
        Main(input, output);
    }
};


class W2T2 : public Task
{
    void Main(std::istream& input, std::ostream& output) override final;

public:
    W2T2() : Task(nullptr) {}
    explicit W2T2(Test* const reference) : Task(reference) {}
};


struct AssociatedValue : private std::pair<int64_t, int64_t>
{
    using LambdaOperator 
        = const std::function<bool(const AssociatedValue&, const AssociatedValue&)>;

    static LambdaOperator less_value;
    static LambdaOperator less_index;

    int64_t& value = std::pair<int64_t, int64_t>::first;
    int64_t& index = std::pair<int64_t, int64_t>::second;

    AssociatedValue() : std::pair<int64_t, int64_t>() {}
    AssociatedValue(const int64_t& value, const int64_t& index) 
        : std::pair<int64_t, int64_t>(value, index) {}

    AssociatedValue(AssociatedValue&& other) noexcept 
        : std::pair<int64_t, int64_t>(std::move(other)) {}

    explicit AssociatedValue(const AssociatedValue& other) 
        : std::pair<int64_t, int64_t>(other.value, other.index) {}

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

AssociatedValue::LambdaOperator AssociatedValue::less_value = 
[](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
    return lhs.value < rhs.value;
};

AssociatedValue::LambdaOperator AssociatedValue::less_index = 
[](const AssociatedValue& lhs, const AssociatedValue& rhs)
{
    return lhs.index < rhs.index;
};

static int64_t LeftBoundSearch(const AssociatedValue* p_begin,
    const AssociatedValue* p_end, const int64_t offset, const int64_t value)
{
    const int64_t length = static_cast<int64_t>(p_end - p_begin);

    int64_t left = offset;
    int64_t right = length - 1U;

    while (right - left > 1)
    {
        int64_t middle = (left + right) / 2;

        if (p_begin[middle].value <= value)
        {
            left = middle;
        }
        else
        {
            right = middle;
        }
    }

    while ((left + 1 < length) && (p_begin[left + 1].value <= value))
    {
        ++left;
    }

    return left;
}

static void PrintIndex(std::ostream& output, 
    const AssociatedValue* p_begin, const AssociatedValue* const p_end)
{
    do
    {
        output << p_begin->index << ' ';

        ++p_begin;
    } while (p_begin < p_end);
}

void W2T2::Main(std::istream& input, std::ostream& output)
{
    std::istream_iterator<int64_t, char> istream(input);

    constexpr int64_t kReservedForSummNull = 1U;

    constexpr int64_t kReserved = kReservedForSummNull;

    const int64_t length = Next(istream);

    if (length == 1)
    {
        output << Next(istream) << '\n';
        output << 1 << ' ';

        return;
    }

    std::byte* p_allocation 
        = new std::byte[sizeof(AssociatedValue) * length 
            + sizeof(int64_t) * (length + kReserved)];

    AssociatedValue* const p_begin_value = reinterpret_cast<AssociatedValue*>(p_allocation);
    AssociatedValue* const p_end_value = reinterpret_cast<AssociatedValue*>(p_allocation) + length;

    AssociatedValue* const p_values = p_begin_value;

    int64_t* const p_sums = reinterpret_cast<int64_t*>(p_end_value);

    for (int64_t index = 0; index < length; ++index)
    {
        new(p_begin_value + index) AssociatedValue(Next(istream), index + 1);
    }

    std::sort(p_begin_value, p_end_value, AssociatedValue::less_value);

    p_sums[0] = 0;

    for (int64_t idx = 0; idx < length; ++idx)
    {
        p_sums[kReserved + idx] = p_sums[idx] + p_values[idx].value;
    }

    int64_t max = p_sums[0];

    AssociatedValue* p_begin_index = p_begin_value;
    AssociatedValue* p_end_index = p_begin_value;

    for (int64_t index = 0; index < length - 1; ++index)
    {
        const int64_t ai = p_values[index + 0U].value;
        const int64_t aj = p_values[index + 1U].value;

        const int64_t bound_idx = LeftBoundSearch(p_begin_value, p_end_value, index, ai + aj);

        const int64_t h_sum = p_sums[bound_idx + 1U];
        const int64_t l_sum = p_sums[index];

        if (h_sum - l_sum > max)
        {
            max = h_sum - l_sum;

            p_begin_index = p_begin_value + index;
            p_end_index = p_begin_value + bound_idx + 1;
        }
    }

    std::sort(p_begin_index, p_end_index, AssociatedValue::less_index);

    output << max << '\n';

    PrintIndex(output, p_begin_index, p_end_index);

    delete[] p_allocation;
}

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    W2T2 task;

    task.Run(std::cin, std::cout);

    return 0;
}
