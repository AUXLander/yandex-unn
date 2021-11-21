#include "W3T1.hpp"

#include <vector>

#include "fixed_set.h"

void readInputToVector(std::istream& input, std::vector<int>& result_vector)
{
    unsigned int number_of_numbers;

    int current_int;

    input >> number_of_numbers;

    result_vector.reserve(number_of_numbers);
    
    for (int i = 0; i < number_of_numbers; ++i)
    {
        input >> current_int;

        result_vector.push_back(current_int);
    }
}

void testNumbers(const FixedSet& fixed_set, const std::vector<int>& numbers_to_test, std::vector<bool>& result_of_test)
{
    for (const auto number : numbers_to_test)
    {
        result_of_test.push_back(fixed_set.Contains(number));
    }
}

void writeOutput(std::ostream& output, const std::vector<bool>& output_vector)
{
    for (const bool value : output_vector)
    {
        output << (value ? "Yes" : "No") << '\n';
    }
}


void W3T1::test(Test* const reference)
{
    if (reference != nullptr)
    {
        reference->open(*this).input("0 1 1").expect("No\n");

        reference->open(*this).input("3 1 2 3 4 1 2 3 4").expect("Yes\nYes\nYes\nNo\n");
        reference->open(*this).input("3 3 1 2 4 10 1 4 2").expect("No\nYes\nNo\nYes\n");

        reference->open(*this).input("4 12 10 3 25 6 1 2 3 4 5 0 ").expect("No\nNo\nYes\nNo\nNo\nNo\n");
        reference->open(*this).input("2 1 1 3 1 3 1 ").expect("Yes\nNo\nYes\n");

        reference->open(*this).input("3 1 1 1  2  1 1 ").expect("Yes\nYes\n");
        reference->open(*this).input("3 -1 -100 2 4 3 2 2 7 ").expect("No\nYes\nYes\nNo\n");
        reference->open(*this).input("6 10 -1 7 12 123 0 4 1 -20 0 -10 ").expect("No\nNo\nYes\nNo\n");
        reference->open(*this).input("30 29 31 2 3 4 5 7 6 8 9 10 12 11 13 14 15 16 17 18 19 20 22 21 23 24 25 26 27 28 30 30 -1 -2 -6 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 20 22 ").expect("No\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nNo\nYes\nYes\n");
        reference->open(*this).input("4 12 10 3 25 6 1 2 3 4 5 0 ").expect("No\nNo\nYes\nNo\nNo\nNo\n");
        reference->open(*this).input("2 1 1 3 1 3 1 ").expect("Yes\nNo\nYes\n");

    }
}

void W3T1::main(std::istream& input, std::ostream& output)
{
    srand(2010);
    std::vector<int> numbers, numbers_to_test;
    std::vector<bool> result_of_test;
    readInputToVector(input, numbers);
    readInputToVector(input, numbers_to_test);
    FixedSet fixed_set;
    fixed_set.Initialize(numbers);
    testNumbers(fixed_set, numbers_to_test, result_of_test);
    writeOutput(output, result_of_test);
}