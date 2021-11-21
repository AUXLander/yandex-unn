#include <iterator>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <utility>
#include <tuple>
#include <random>
#include <memory>
#include <numeric>
#include <functional>

template<class Z>
struct triplet_t
{
    Z aa;
    Z bb;
    Z cc;

    triplet_t(const Z& aa, const Z& bb, const Z& cc)
        : aa(aa), bb(bb), cc(cc)
    {;}

    ~triplet_t()
    {;}

    inline bool operator== (const triplet_t<Z>& other) const
    {
        return (other.aa == aa) && (other.bb == bb) && (other.cc == cc);
    }
};

template <typename T>
inline T gcd(T a, T b)
{
    while (b != 0) 
    {
        std::swap(b, a %= b);
    }
    return a;
}

template<typename T, typename H>
struct signature_t
{
    using triplet = triplet_t<H>;

    const triplet sign;

    signature_t(const T a, const T b, const T c)
        : sign(this->process(a, b, c))
    {;}

    signature_t(const signature_t& other)
        : sign(other.sign)
    {;}

    signature_t(signature_t&& other) noexcept
        : sign(std::move(other.sign))
    {;}

    ~signature_t()
    {;}

    inline bool operator== (const signature_t<T, H>& other) const
    {
        return sign == other.sign;
    }

    triplet process(T, T, T)
    {
        return triplet();
    };
};

using sign_key_type = unsigned int;
using signature = signature_t<sign_key_type, unsigned int>;

class signature_table
{
    class sub_signature_table
    {
    public:
        using Node = std::pair<signature, size_t>;

        using bucket = std::vector<Node>;

    private:
        size_t size;
        std::vector<std::unique_ptr<bucket>> htable;

    public:
        explicit sub_signature_table(const size_t size = 1000)
            : size(size), htable(size)
        {;}

        ~sub_signature_table()
        {;}

        inline size_t hash_index(const signature& key)
        {
            const auto ha = std::hash<unsigned short>{}(key.sign.aa);
            const auto hb = std::hash<unsigned short>{}(key.sign.bb);
            const auto hc = std::hash<unsigned short>{}(key.sign.cc);

            const auto hash = ha ^ (hb << 1) ^ (hc << 3);
            const auto index = hash % size;

            return index;
        }

        void add(const signature& value)
        {
            const auto index = hash_index(value);

            if (htable[index])
            {
                for (size_t idx = 0; idx < htable[index]->size(); ++idx)
                {
                    if (htable[index]->data()[idx].first == value)
                    {
                        ++htable[index]->data()[idx].second;
                        return;
                    }
                }
            }
            else
            {
                htable[index] = std::unique_ptr<bucket>(new bucket());
            }

            htable[index]->emplace_back(value, 1);

            return;
        }

        size_t not_empty_buckets() const
        {
            size_t count = 0;

            for (const auto& bucket : htable)
            {
                if (bucket)
                {
                    count += bucket->size();
                }
            }

            return count;
        }
    };


    size_t size;
    std::vector<sub_signature_table> htable;
public:
    explicit signature_table(const size_t size = 1000)
        : size(size), htable(size)
    {;}

    inline size_t hash_index(const signature& key)
    {
        const auto ha = std::hash<unsigned int>{}(key.sign.aa);
        const auto hb = std::hash<unsigned int>{}(key.sign.bb);
        const auto hc = std::hash<unsigned int>{}(key.sign.cc);

        const auto hash = (ha << 0) ^ (hb << 1) ^ (hc << 3);
        const auto index = hash % size;

        return index;
    }

    void add(const signature& value)
    {
        const auto index = hash_index(value);

        return htable[index].add(value);
    }

    size_t not_empty_buckets()
    {
        size_t count = 0;

        for (const auto& bucket : htable)
        {
            count += bucket.not_empty_buckets();
        }

        return count;
    }
};

template<>
signature::triplet signature::process(
    sign_key_type aa,
    sign_key_type bb,
    sign_key_type cc)
{
    if (aa > bb) { std::swap(aa, bb); }
    if (bb > cc) { std::swap(bb, cc); }
    if (aa > bb) { std::swap(aa, bb); }

    const auto normalize = gcd(gcd(aa, bb), cc);

    signature::triplet object(aa / normalize, bb / normalize, cc / normalize);

    return object;
}

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t length;
    sign_key_type la = 0, lb = 0, lc = 0;
    signature_table table(1000);

    std::cin >> length;

    for (size_t index = 0; index < length; ++index)
    {
        std::cin >> la >> lb >> lc;

        signature key(la, lb, lc);

        table.add(key);
    }

    std::cout << table.not_empty_buckets() << '\n';
    return 0;
}

