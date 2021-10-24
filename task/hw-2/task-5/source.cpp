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


struct Path : private std::pair<int, bool>
{
    int& distance = std::pair<int, bool>::first;
    bool& isVisited = std::pair<int, bool>::second;

    Path() : std::pair<int, bool>()
    {;}

    Path(const int coordinate, const bool isVisited = false)
        : std::pair<int, bool>(coordinate, isVisited)
    {;}

    explicit Path(const Path& other)
        : std::pair<int, bool>(other.distance, other.isVisited)
    {;}

    Path(Path&& other) noexcept
        : std::pair<int, bool>(std::move(other))
    {;}
};

struct AliPoint : private std::pair<int, int>
{
    int& coordinate = std::pair<int, int>::first;
    int& lifetime = std::pair<int, int>::second;

    AliPoint() : std::pair<int, int>()
    {;}

    AliPoint(const int coordinate, const int lifetime)
        : std::pair<int, int>(coordinate, lifetime)
    {;}

    explicit AliPoint(const AliPoint& other)
        : std::pair<int, int>(other.coordinate, other.lifetime)
    {;}

    AliPoint(AliPoint&& other) noexcept
        : std::pair<int, int>(std::move(other))
    {;}

    inline AliPoint& operator=(const AliPoint& other)
    {
        this->lifetime = other.lifetime;
        this->coordinate = other.coordinate;

        return *this;
    }

    inline bool operator<(const AliPoint& other) const
    {
        return this->coordinate < other.coordinate;
    }
};

struct Explore
{
    using GraphTable = std::vector<std::vector<Path>>;

    std::vector<AliPoint> coins;

    GraphTable leftTable;
    GraphTable rightTable;

    explicit Explore(const size_t count) 
        : coins(count), 
          leftTable(count, std::vector<Path>(count)), 
          rightTable(count, std::vector<Path>(count))
    {;}

    int left(const int left, const int right);
    int right(const int left, const int right);

    int process();

    static inline int distance(const AliPoint& lhs, const AliPoint& rhs);

    static int check_vertex(const int res_left,
                            const int add_left, 
                            const int res_right, 
                            const int add_right);
};

class W2T5 : public Task
{
    void test(Test* const reference) override final {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    W2T5() : Task(nullptr) {}
    explicit W2T5(Test* const reference) : Task(reference) {}
};


int Explore::distance(const AliPoint& lhs, const AliPoint& rhs)
{
    return lhs.coordinate - rhs.coordinate;
}

int Explore::check_vertex(const int res_left, 
                          const int add_left, 
                          const int res_right, 
                          const int add_right)
{
    if (res_left != -1 && res_right != -1)
    {
        return std::min(res_left + add_left, res_right + add_right);
    }

    if (res_left != -1)
    {
        return res_left + add_left;
    }

    if (res_right != -1)
    {
        return res_right + add_right;
    }

    return -1;
}

int Explore::left(const int left, const int right)
{
    auto& table = leftTable;

    if (table[left][right].isVisited == true)
    {
        return table[left][right].distance;
    }

    if (left == right)
    {
        return 0;
    }

    int result = -1;

    const int res_left = Explore::left(left + 1, right);
    const int add_left = Explore::distance(coins[left + 1], coins[left]);

    const int res_right = Explore::right(left + 1, right);
    const int add_right = Explore::distance(coins[right], coins[left]);

    const int res_t = check_vertex(res_left, add_left, res_right, add_right);

    if ((res_t != -1) && (coins[left].lifetime >= res_t))
    {
        result = res_t;
    }

    table[left][right].distance = result;
    table[left][right].isVisited = true;

    return result;
}

int Explore::right(const int left, const int right)
{
    auto& table = rightTable;

    if (left == right)
    {
        return 0;
    }

    if (table[left][right].isVisited == true)
    {
        return table[left][right].distance;
    }

    int result = -1;

    const int res_right = Explore::right(left, right - 1);
    const int add_right = Explore::distance(coins[right], coins[right - 1]);

    const int res_left = Explore::left(left, right - 1);
    const int add_left = Explore::distance(coins[right], coins[left]);

    int res_t = check_vertex(res_right, add_right, res_left, add_left);

    if ((res_t != -1) && (coins[right].lifetime >= res_t))
    {
        result = res_t;
    };

    table[left][right].distance = result;
    table[left][right].isVisited = true;

    return result;
}

int Explore::process()
{
    if (coins.empty())
    {
        return 0;
    }

    int ans_left = Explore::left(0, coins.size() - 1);
    int ans_right = Explore::right(0, coins.size() - 1);

    if ((ans_left != -1) && (ans_right != -1))
    {
        return std::min(ans_left, ans_right);
    }

    if (ans_left != -1)
    {
        return ans_left;
    }

    if (ans_right != -1)
    {
        return ans_right;
    }

    return -1;
}

void W2T5::main(std::istream& input, std::ostream& output)
{
    size_t count = 0;
    input >> count;

    Explore solution(count);

    for (auto& coin : solution.coins)
    {
        input >> coin.coordinate >> coin.lifetime;
    }

    std::sort(solution.coins.begin(), solution.coins.end());

    int min_time = solution.process();

    if (min_time == -1)
    {
        output << "No solution" << '\n';
    }
    else
    {
        output << min_time << '\n';
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
