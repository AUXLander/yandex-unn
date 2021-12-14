#pragma once
#include <iostream>
#include <string>
#include <deque>
#include <utility>
#include <iterator>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <random>

// #define struct_by_pair(name, nfirst, nsecond, type_first, type_second)\
//    struct name : public std::pair<type_first, type_second> \
//    {\
//        type_first &nfirst = std::pair<type_first, type_second>::first; \
//        type_second &nsecond = std::pair<type_first, type_second>::second; \
//        name() : std::pair<type_first, type_second>() {;}\
//        name(const type_first &nfirst, const type_second &nsecond) \
//            : std::pair<type_first, type_second>(nfirst, nsecond) {;} \
//        name(const name &other) \
//            : std::pair<type_first, type_second>(other) {;} \
//        name(name &&other) \
//            : std::pair<type_first, type_second>(std::move(other)) {;} \
//        inline name& operator=(const name &other) {\
//            this->nfirst = other.nfirst;\
//            this->nsecond = other.nsecond;\
//            return *this;\
//        }\
//    }

template<class Y> Y Sqr(const Y& p_root) { return p_root * p_root; }

constexpr int64_t kGRandMax = 2147483647;

static std::random_device rd;
static std::mt19937 rng{ rd() };

static std::uniform_int_distribution<size_t> uid(0, kGRandMax);

// struct_by_pair(Coefficients, a, b, size_t, size_t);
struct Coefficients : public std::pair<size_t, size_t>
{
    size_t& xx = std::pair<size_t, size_t>::first;
    size_t& yy = std::pair<size_t, size_t>::second;

    Coefficients() : std::pair<size_t, size_t>() { ; }

    Coefficients(const size_t& xx, const size_t& yy)
        : std::pair<size_t, size_t>(xx, yy) { ; }

    Coefficients(const Coefficients& other)
        : std::pair<size_t, size_t>(other) { ; }

    Coefficients(Coefficients&& other)
        : std::pair<size_t, size_t>(std::move(other)) { ; }

    inline Coefficients& operator=(const Coefficients& other)
    {
        this->xx = other.xx;
        this->yy = other.yy;
        return *this;
    }
};

static inline void GenerateNext(const Coefficients& coefficient)
{
    coefficient.xx = uid(rng) % kGRandMax;
    coefficient.yy = uid(rng) % kGRandMax;
}

static inline size_t HashFunction(const Coefficients& coefficient, const int64_t p_root)
{
    return (((coefficient.xx * p_root + coefficient.yy) % kGRandMax) + kGRandMax) % kGRandMax;
}

static inline size_t HashIndex(const Coefficients& coefficient,
                                const int64_t p_root,
                                const size_t limit)
{
    return HashFunction(coefficient, p_root) % limit;
}

class FixedSet
{
    struct HashTableBase
    {
        Coefficients coefficient;

        // struct_by_pair(Cell, value, is_empty, int, bool);
        struct Cell : public std::pair<int, bool>
        {
            int& p_root = std::pair<int, bool>::first;
            bool& is_empty = std::pair<int, bool>::second;
            Cell() : std::pair<int, bool>() { ; }
            Cell(const int& p_root, const bool& is_empty)
                : std::pair<int, bool>(p_root, is_empty) { ; }
            Cell(const Cell& other)
                : std::pair<int, bool>(other) { ; }
            Cell(Cell&& other)
                : std::pair<int, bool>(std::move(other)) { ; }
            inline Cell& operator=(const Cell& other) 
            {

                this->p_root = other.p_root;
                this->is_empty = other.is_empty;
                return *this;
            }
        };

        static bool SetIfEmpty(Cell& cell, const Cell::first_type& p_root)
        {
            if (cell.is_empty == true)
            {
                cell.p_root = p_root;
                cell.is_empty = false;

                return true;
            }

            return cell.p_root == p_root;
        }

        HashTableBase() : coefficient() {}

        virtual void Prepare() = 0;
    };

    struct ForegroundHT : public HashTableBase
    {
        struct BackgroundHT : public HashTableBase
        {
            // хранит значения, которые нужно сохранить в этой таблице
            std::vector<int> values_to_store;
            std::vector<Cell> cells;

            BackgroundHT() : HashTableBase() {}

            void Prepare() override
            {
                const auto sqrsize = Sqr(values_to_store.size());

                cells.resize(sqrsize);

                Clear();

                GenerateNext(coefficient);
            }

            void Clear()
            {
                for (auto& cell : cells)
                {
                    cell.is_empty = true;
                }
            }
        };

        std::vector<BackgroundHT> background_table;

        ForegroundHT() : HashTableBase() {}

        // Возвращает квадрат числа сохраненных значений
        size_t MemoryToKeepNumbers() const
        {
            size_t result = 0;

            for (auto& sht : background_table)
            {
                result += Sqr(sht.values_to_store.size());
            }

            return result;
        }

        void CleanStore()
        {
            for (auto& sht : background_table)
            {
                sht.values_to_store.clear();
            }
        }

        void Prepare() override
        {
            for (auto& sht : background_table)
            {
                sht.Prepare();
            }
        }

        void Fill()
        {
            for (auto& sht : background_table)
            {
                bool success = false;

                while (!success)
                {
                    GenerateNext(sht.coefficient);

                    // Clear stored numbers.
                    sht.Clear();

                    // Try to refill secondary hash table.
                    success = true;

                    for (const auto number : sht.values_to_store)
                    {
                        const size_t index = HashIndex(sht.coefficient, 
                            number, sht.values_to_store.size());

                        if (SetIfEmpty(sht.cells[index], number) == false)
                        {
                            // Fail. Collision again.
                            success = false;
                            break;
                        }
                    }
                }
            }
        }
    };

    static constexpr size_t kMagicConstant = 4;

    ForegroundHT foreground_table_;

    size_t size_;

public:
    explicit FixedSet()
        : size_(0)
    {;}

    void Initialize(const std::vector<int>& numbers)
    {
        size_ = numbers.size();

        if (size_ > 0)
        {
            auto& primary = foreground_table_;
            auto& secondary = primary.background_table;

            secondary.resize(size_);

            do {
                // генерируем два случайных числа
                GenerateNext(primary.coefficient);

                // очищаем векторы в каждой второстепенной хеш таблице
                primary.CleanStore();

                // каждое введенное число
                for (const auto number : numbers)
                {
                    // отправляем в хеш функцию с указанными случайными коэффициентами
                    const auto hash = HashFunction(primary.coefficient, number);
                    // хеш обращаем в индексы
                    const auto index = hash % size_;

                    // общаемся к второстепенной таблице по индексу и записываем туда число
                    secondary[index].values_to_store.push_back(number);
                }

                // продолжаем заполнять второстепенные таблицы по хеш 
                // индексам от случайных коэффициентов, 
                // пока сумма квадратов количества чисел во второстепенных таблицах
                // будет больше, чем 4 * количество чисел
            } while (primary.MemoryToKeepNumbers() > (kMagicConstant * size_));

            primary.Prepare();
            primary.Fill();
        }
    }

    bool Contains(int number) const
    {
        if (size_ > 0)
        {
            const auto& primary = foreground_table_;
            const auto  primary_hash = HashFunction(primary.coefficient, number);
            const auto  primary_shift = primary_hash % size_;

            const auto& secondary = primary.background_table[primary_shift];

            if (secondary.values_to_store.empty())
            {
                return false;
            }

            const auto index = HashIndex(secondary.coefficient, number,
                    secondary.values_to_store.size());

            return (secondary.cells[index].is_empty == false)
                    && (secondary.cells[index].p_root == number);
        }

        return false;
    }
};
