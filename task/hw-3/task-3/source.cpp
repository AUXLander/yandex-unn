#include <iterator>
#include <iostream>

#include <cstddef>
#include <utility>
#include <algorithm>
#include <functional>
#include <vector>
#include <limits>
#include <string>
#include <deque>

class Test {};

class Task
{
protected:
    Test* const m_reference;

    virtual void test(Test* const reference) = 0;
    virtual void main(std::istream& input, std::ostream& output) = 0;

public:
    explicit Task(Test* const reference) : m_reference(reference) {};

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
        main(input, output);
    }
};

class W3T3 : public Task
{
    void test(Test* const reference) {};
    void main(std::istream& input, std::ostream& output) override final;

public:
    W3T3() : Task(nullptr) {}
    explicit W3T3(Test* const reference) : Task(reference) {}
};


struct Segment
{
    Segment* prev;
    Segment* next;

    bool free;
    int len, rig, pp;

    Segment(Segment* prev, Segment* next, bool free, int len, int rig, int pp)
        : prev(prev), next(next), free(free),
        len(len), rig(rig), pp(pp)
    {
        link(prev, next);
    }

    inline void link(Segment* prev, Segment* next)
    {
        if (prev)
        {
            prev->next = this;
        }

        if (next)
        {
            next->prev = this;
        }
    }

    inline void unlink()
    {
        if (prev)
        {
            prev->next = next;
        }

        if (next)
        {
            next->prev = prev;
        }
    }
};


struct SegmentManager
{
    constexpr static size_t minn = 1;
    constexpr static size_t maxn = 2147483647;
    constexpr static size_t minm = 1;
    constexpr static size_t maxm = 100'000;

    size_t nn;
    size_t kk;
    int length;

    std::ostream& out;

    enum class State : uint8_t
    {
        declined = 0,
        allocated,
        removed
    };

    State* rs;

    std::vector<Segment*> hh;
    std::vector<Segment*> rig;

    SegmentManager(const size_t nn, std::ostream& out)
        : nn(nn), kk(0), length(1), out(out)
    {
        rs = new State[maxm];

        hh.resize(maxm, nullptr);
        rig.resize(maxm, nullptr);

        hh[0] = new Segment(0, 0, true, 0, nn, 0);
    }

    ~SegmentManager()
    {
        for (auto& si : hh)
        {
            if (si != nullptr)
            {
                delete si;

                auto t = std::find(rig.begin(), rig.end(), si);

                if (t != rig.end())
                {
                    *t = nullptr;
                }

                si = nullptr;
            }
        }

        for (auto& si : rig)
        {
            if (si != nullptr)
            {
                delete si;

                si = nullptr;
            }
        }

        delete[] rs;
    }

    void swap(int aa, int bb)
    {
        std::swap(hh[aa], hh[bb]);

        hh[aa]->pp = aa;
        hh[bb]->pp = bb;
    }

    bool better(int aa, int bb)
    {
        return (hh[aa]->rig - hh[aa]->len) > (hh[bb]->rig - hh[bb]->len);
    }

    void heap_make(int pivot)
    {
        for (size_t index = 0; index < 10000; ++index)
        {
            int query = pivot;

            const auto shift_l = (pivot << 1) + 1;
            const auto shift_r = (pivot << 1) + 2;

            if ((shift_l < length) && better(shift_l, query))
            {
                query = shift_l;
            }

            if ((shift_r < length) && better(shift_r, query))
            {
                query = shift_r;
            }

            if (pivot == query)
            {
                return;
            }

            swap(pivot, query);

            pivot = query;
        }
    }

    void heap_remove()
    {
        --length;

        if (length)
        {
            swap(0, length);
            heap_make(0);
        }
    }

    void heap_move(int index)
    {
        const auto move = [&index]() -> int
        {
            return (index - 1) >> 1;
        };

        while (index && better(index, move()))
        {
            swap(index, move());
            index = move();
        }
    }

    void heap_remove(int aa)
    {
        swap(aa, length - 1);

        --length;

        if (aa >= length)
        {
            return;
        }

        heap_move(aa);

        heap_make(aa);
    }

    void heap_add(Segment* seg)
    {
        seg->pp = length;

        hh[length] = seg;

        heap_move(length++);
    }

    void allocate(int index)
    {
        Segment*& cc = hh[0];

        if (!length || !cc || ((cc->rig - cc->len) < index))
        {
            rs[kk++] = State::declined;

            out << "-1\n";

            return;
        }

        rs[kk++] = State::allocated;
        rig[kk - 1] = new Segment(cc->prev, cc, false, cc->len, cc->len + index, -1);

        out << (cc->len + 1) << '\n';

        cc->len += index;
        if (cc->len < cc->rig)
        {
            heap_make(cc->pp);
        }
        else
        {
            cc->unlink();

            heap_remove();

            delete cc;
            cc = nullptr;
        }
    }

    void remove(int index)
    {
        --index;

        rs[kk++] = State::removed;

        if (rs[index] == State::declined)
        {
            return;
        }

        rs[index] = State::removed;

        Segment*& cc = rig[index];
        Segment*& sp = cc->prev;
        Segment*& sn = cc->next;

        bool bp = sp && sp->free;
        bool bn = sn && sn->free;

        if (!bp && !bn)
        {
            cc->free = true;
            heap_add(cc);
            return;
        }

        if (!bp)
        {
            sn->len = cc->len;
            heap_move(sn->pp);
            cc->unlink();
            delete cc;
            cc = nullptr;
            return;
        }

        if (!bn)
        {
            sp->rig = cc->rig;
            heap_move(sp->pp);
            cc->unlink();
            delete cc;

            cc = nullptr;
            return;
        }

        sp->rig = sn->rig;
        heap_move(sp->pp);
        cc->unlink();
        delete cc;
        cc = nullptr;

        heap_remove(sn->pp);
        sn->unlink();
        delete sn;

        sn = nullptr;
    }
};

void W3T3::main(std::istream& input, std::ostream& output)
{
    size_t nn, mm;

    input >> nn >> mm;

    SegmentManager sm(nn, output);

    int value;
    for (size_t index = 0; index < mm; ++index)
    {
        input >> value;

        if (value > 0)
        {
            sm.allocate(value);
        }
        else
        {
            sm.remove(-value);
        }
    }
}

int main(int, char* [])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    W3T3 task;

    task.run(std::cin, std::cout);

    return 0;
}