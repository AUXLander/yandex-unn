#include "w2t4.hpp"

unsigned int cur = 0;// беззнаковое 32-битное число  

unsigned int nextRand24(unsigned int a_numb, unsigned int b_numb)
{
    cur = cur * a_numb + b_numb; // вычисляется с переполнениями  
    return cur >> 8; // число от 0 до 2**24-1.  
}

unsigned int nextRand32(unsigned int a_numb, unsigned int b_numb)
{
    unsigned int a_ = nextRand24(a_numb, b_numb), b_ = nextRand24(a_numb, b_numb);
    return (a_ << 8) ^ b_; // число от 0 до 2**32-1.  
}

struct Postman
{
    constexpr static size_t keygen = 72875;

    std::deque<uint32_t> input;

    std::mt19937 generator;
    std::uniform_int_distribution<uint32_t> dist;

    explicit Postman(const size_t size, const size_t a_numb, const size_t b_numb)
        : input(size), generator(keygen), dist(0, size - 1)
    {
        for (auto& element : input)
        {
            element = nextRand32(a_numb, b_numb);
        }
    }

    inline uint32_t generate(const size_t size)
    {
        std::uniform_int_distribution<uint32_t> dist(0, size - 1);

        return dist(generator);
    }

    void initialize_next(
        const std::deque<uint32_t>& vector,
        const uint32_t pivotIdx, 
        std::deque<uint32_t>& levector,
        std::deque<uint32_t>& gevector,
        std::deque<uint32_t>& pevector)
    {
        for (const auto& element : vector)
        {
            if (element < vector[pivotIdx])
            {
                levector.push_back(element);
            }
            else if (element > vector[pivotIdx])
            {
                gevector.push_back(element);
            }
            else
            {
                pevector.push_back(element);
            }
        }
    }

    uint32_t select(const std::deque<uint32_t>& input, const size_t k_numb)
    {
        if (input.size() == 1)
        {
            return input[0];
        }

        std::deque<uint32_t> gevector;
        std::deque<uint32_t> levector;
        std::deque<uint32_t> pevecot;

        uint32_t pivot = generate(input.size());

        initialize_next(input, pivot, levector, gevector, pevecot);

        if (k_numb < levector.size())
        {
            return select(levector, k_numb);
        }
        else if (k_numb < (levector.size() + pevecot.size()))
        {
            return pevecot[0];
        }
        else
        {
            return select(gevector, k_numb - levector.size() - pevecot.size());
        }
    }
};

void W2T4::main(std::istream& input, std::ostream& output)
{
    size_t n_size, a_number, b_number;

    input >> n_size >> a_number >> b_number;

    Postman reshalo(n_size, a_number, b_number);

    uint32_t y_coord = reshalo.select(reshalo.input, reshalo.input.size() / 2);

    int64_t sum = 0;
    
    for (uint32_t x_coord : reshalo.input)
    {
        sum += std::abs(static_cast<int64_t>(y_coord) - static_cast<int64_t>(x_coord));
    }

    output << sum;
}

void W2T4::test(Test* const reference)
{
    if (reference != nullptr)
    {
        this->run(std::cin, std::cout);
    }
} 