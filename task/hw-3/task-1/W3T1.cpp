#include "W3T1.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <utility>
#include <iterator>
#include <cstdio>
#include <vector>
#include <cstdlib>

#define struct_by_pair(name, nfirst, nsecond, type_first, type_second)\
    struct name : public std::pair<type_first, type_second> \
    {\
        type_first &nfirst = std::pair<type_first, type_second>::first; \
        type_second &nsecond = std::pair<type_first, type_second>::second; \
        name() : std::pair<type_first, type_second>() {;}\
        name(const type_first &nfirst, const type_second &nsecond) \
            : std::pair<type_first, type_second>(nfirst, nsecond) {;} \
    }


struct_by_pair(Coefficients, a, b, long long, long long);

using std::vector;

template<class Y> Y sqr(const Y& y) { return y * y; }

constexpr long long g_BigPrimeNumber = 2147483647;

//inline long long hashFunction(long long a_coefficient, long long b_coefficient, long long x) 
//{
//    return (((a_coefficient * x + b_coefficient) % g_BigPrimeNumber) + g_BigPrimeNumber) % g_BigPrimeNumber;
//}

inline long long hashFunction(const Coefficients& coefficient, long long x)
{
    return (((coefficient.a * x + coefficient.b) % g_BigPrimeNumber) + g_BigPrimeNumber) % g_BigPrimeNumber;
}

inline void generateHashFunctionCoefficients(long long &a, long long &b)
{
    a = rand() % (g_BigPrimeNumber - 1) + 1;
    b = rand() % (g_BigPrimeNumber);
}


class FixedSet
{
    struct PrimaryHashTable
    {
        Coefficients coefficient;

        struct SecondaryHashTable
        {
            Coefficients coefficient;

            vector<int> numbers_to_store;
            vector<int> numbers;
            vector<int> numbers_empty;

            SecondaryHashTable() : coefficient() {}
        };

        vector<SecondaryHashTable> secondary_hash_tables_;

        // Returns number of cell needed to keep hash table.
        size_t memoryToKeepNumbers() const
        {
            size_t result = 0;

            for (auto sht = secondary_hash_tables_.begin(); sht != secondary_hash_tables_.end(); ++sht)
            {
                result += sqr(sht->numbers_to_store.size());
            }

            return result;
        }

        void cleanNumbersToStore()
        {
            for (auto sht = secondary_hash_tables_.begin(); sht != secondary_hash_tables_.end(); ++sht)
            {
                sht->numbers_to_store.clear();
            }
        }

        // Resizes each secondary hash table to numbers_of_numbers_^2 size and
        // generates coefficients a and b for it.
        void prepareSecondaryHashTables()
        {
            for (auto sht_iter = secondary_hash_tables_.begin(); sht_iter != secondary_hash_tables_.end(); ++sht_iter)
            {
                sht_iter->numbers.resize(sqr(sht_iter->numbers_to_store.size()));

                sht_iter->numbers_empty.resize(sqr(sht_iter->numbers_to_store.size()), true);

                generateHashFunctionCoefficients(sht_iter->coefficient.a, sht_iter->coefficient.b);
            }
        }

        void fillHashTable()
        {
            for (auto sht_iter = secondary_hash_tables_.begin(); sht_iter != secondary_hash_tables_.end(); ++sht_iter)
            { 
                bool success = false;

                while (!success)
                {
                    generateHashFunctionCoefficients(sht_iter->coefficient.a, sht_iter->coefficient.b);

                    // Clear stored numbers.
                    sht_iter->numbers_empty.assign(sht_iter->numbers_empty.size(), true);

                    // Try to refill secondary hash table.
                    success = true;
                    for (auto number_iter = sht_iter->numbers_to_store.begin(); number_iter != sht_iter->numbers_to_store.end(); ++number_iter)
                    {
                        long long hash_in_sht = hashFunction(sht_iter->coefficient, *number_iter) % sht_iter->numbers_to_store.size();

                        if (sht_iter->numbers_empty[hash_in_sht])
                        {
                            sht_iter->numbers[hash_in_sht] = *number_iter;
                            sht_iter->numbers_empty[hash_in_sht] = false;
                        }
                        else if (sht_iter->numbers[hash_in_sht] != *number_iter)
                        {
                            // Fail. Collision again.
                            success = false;
                            break;
                        }
                    }
                }
            }
        }

        PrimaryHashTable() : coefficient() {}
    };

    PrimaryHashTable primary_hash_table_;

    int total_number_of_numbers_;

    static constexpr size_t k_MemoryLimitMultiplier = 4;

public:

    void preparePrimaryHashTable(const vector<int>& numbers)
    {
        auto &primary = primary_hash_table_;
        auto& secondary = primary.secondary_hash_tables_;

        secondary.resize(numbers.size());

        do {
            generateHashFunctionCoefficients(primary.coefficient.a, primary.coefficient.b);

            primary.cleanNumbersToStore();

            for (const auto number : numbers)
            {
                const auto hash = hashFunction(primary.coefficient, number);
                const auto index = hash % numbers.size();

                secondary[index].numbers_to_store.push_back(number);
            }

        } while (primary.memoryToKeepNumbers() > (k_MemoryLimitMultiplier * numbers.size()));
    }

    void Initialize(const vector<int>& numbers)
    {
        total_number_of_numbers_ = numbers.size();
        preparePrimaryHashTable(numbers);
        primary_hash_table_.prepareSecondaryHashTables();
        primary_hash_table_.fillHashTable();
    }

    explicit FixedSet() 
        : total_number_of_numbers_(0)
    {;}

    bool Contains(int number) const 
    {
        const auto &primary = primary_hash_table_;
        const auto  primary_hash = hashFunction(primary.coefficient, number);
        const auto  primary_shift = primary_hash % total_number_of_numbers_;

        const auto &secondary = primary.secondary_hash_tables_[primary_shift];

        if (secondary.numbers_to_store.size() == 0)
        {
            return false;
        }

        const auto secondary_hash = hashFunction(secondary.coefficient, number) % secondary.numbers_to_store.size();

        return (!secondary.numbers_empty[secondary_hash] && secondary.numbers[secondary_hash] == number);
    }
};

void readInputToVector(vector<int>& result_vector)
{
    int number_of_numbers, current_int;

    std::cin >> number_of_numbers;
    
    for (int i = 0; i < number_of_numbers; ++i) {

        std::cin >> current_int;

        result_vector.push_back(current_int);
    }
}

void testNumbers(const FixedSet& fixed_set, const vector<int>& numbers_to_test, vector<bool>& result_of_test) 
{
    for (const auto number : numbers_to_test)
    {
        result_of_test.push_back(fixed_set.Contains(number));
    }
}

void writeOutput(const vector<bool>& output_vector) 
{
    for (vector<bool>::const_iterator it = output_vector.begin(); it != output_vector.end(); ++it)
    {
        *it ? printf("Yes\n") : printf("No\n");
    }
}


void W3T1::test(Test* const reference)
{

}

void W3T1::main(std::istream& input, std::ostream& output)
{
    srand(2010);
    vector<int> numbers, numbers_to_test;
    vector<bool> result_of_test;
    readInputToVector(numbers);
    readInputToVector(numbers_to_test);
    FixedSet fixed_set;
    fixed_set.Initialize(numbers);
    testNumbers(fixed_set, numbers_to_test, result_of_test);
    writeOutput(result_of_test);
    
}