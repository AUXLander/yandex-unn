#include "W3T3.h"

#include <vector>
#include <list>
#include <algorithm>
#include <cstdio>
#include <iostream>

using std::list;
using std::swap;
using std::vector;

struct Chunk {
    int start_;
    int length_;
    bool is_free_;
    int index_;

    Chunk(int start = 0, int length = 0, bool is_free = false, int index = 0)
        : start_(start), length_(length), is_free_(is_free), index_(index) {
    }

    bool operator<(const Chunk& other) const {
        if (length_ < other.length_) {
            return true;
        }
        if (length_ == other.length_) {
            return start_ > other.start_;
        }
        return false;
    }

    bool operator==(const Chunk& other) const {
        return start_ == other.start_ && length_ == other.length_ && is_free_ == other.is_free_ &&
            index_ == other.index_;
    }

    bool operator>(const Chunk& other) const {
        return other < *this;
    }
};

class MaxChunkHeap {
private:
    vector<list<Chunk>::iterator> array_;

public:
    bool Empty() const {
        return array_.empty();
    }

    void Heapify(int root_index) {
        int left_son_index, right_son_index, largest_index;
        left_son_index = 2 * root_index + 1;
        right_son_index = 2 * root_index + 2;
        largest_index = root_index;
        if (left_son_index < static_cast<int>(array_.size()) &&
            *(array_[left_son_index]) > *(array_[largest_index])) {
            largest_index = left_son_index;
        }
        if (right_son_index < static_cast<int>(array_.size()) &&
            *(array_[right_son_index]) > *(array_[largest_index])) {
            largest_index = right_son_index;
        }
        if (largest_index != root_index) {
            swap(array_[root_index]->index_, array_[largest_index]->index_);
            swap(array_[root_index], array_[largest_index]);
            Heapify(largest_index);
        }
    }

    void ChangeKey(int index, list<Chunk>::iterator new_key) {
        array_[index] = new_key;
        new_key->index_ = index;
        Heapify(index);
        while (index > 0 && *(array_[(index - 1) / 2]) < *(array_[index])) {
            swap(array_[(index - 1) / 2]->index_, array_[index]->index_);
            swap(array_[(index - 1) / 2], array_[index]);
            index = (index - 1) / 2;
        }
    }

    void Insert(list<Chunk>::iterator new_key) {
        array_.push_back(new_key);
        ChangeKey(array_.size() - 1, new_key);
    }

    list<Chunk>::iterator GetMax() {
        return array_[0];
    }

    list<Chunk>::iterator ExtractMax() {
        list<Chunk>::iterator max_element = array_[0];
        array_[0] = array_[array_.size() - 1];
        array_[0]->index_ = 0;
        array_.pop_back();
        Heapify(0);
        return max_element;
    }

    void Remove(int index) {
        if (array_.empty()) {
            return;
        }
        array_[index] = array_[array_.size() - 1];
        array_[index]->index_ = index;
        array_.pop_back();
        ChangeKey(index, array_[index]);
    }

    MaxChunkHeap() {
    }
};

class Allocator {
private:
    MaxChunkHeap memory_heap_;
    list<Chunk> chunks_list_;
    vector<list<Chunk>::iterator> allocated_chunks_;
    int operations_counter_;

public:
    explicit Allocator(int size_of_memory) {
        chunks_list_.push_back(Chunk(1, size_of_memory, true, 0));
        memory_heap_.Insert(chunks_list_.begin());
        operations_counter_ = 0;
    }

    int AllocateMemory(int size) {
        ++operations_counter_;
        allocated_chunks_.resize(operations_counter_, chunks_list_.end());

        if (memory_heap_.Empty()) {
            return -1;
        }

        if (memory_heap_.GetMax()->length_ < size) {
            return -1;
        }

        list<Chunk>::iterator free_chunk = memory_heap_.ExtractMax();
        Chunk new_chunk(free_chunk->start_, size, false);
        allocated_chunks_[operations_counter_ - 1] = chunks_list_.insert(free_chunk, new_chunk);

        if (free_chunk->length_ == size) {
            chunks_list_.erase(free_chunk);
            return allocated_chunks_[operations_counter_ - 1]->start_;
        }

        free_chunk->length_ -= size;
        free_chunk->start_ += size;
        memory_heap_.Insert(free_chunk);
        return allocated_chunks_[operations_counter_ - 1]->start_;
    }

    void DeallocateMemory(int num) {
        ++operations_counter_;
        if (allocated_chunks_[num - 1] == chunks_list_.end()) {
            return;
        }

        allocated_chunks_[num - 1]->is_free_ = true;

        // Merge with prev
        if (allocated_chunks_[num - 1] != chunks_list_.begin()) {
            list<Chunk>::iterator prev_chunk = allocated_chunks_[num - 1];
            --prev_chunk;
            if (prev_chunk->is_free_) {
                allocated_chunks_[num - 1]->length_ += prev_chunk->length_;
                allocated_chunks_[num - 1]->start_ = prev_chunk->start_;
                memory_heap_.Remove(prev_chunk->index_);
                chunks_list_.erase(prev_chunk);
            }
        }
        // Merge with next
        if (allocated_chunks_[num - 1] != --(chunks_list_.end())) {
            list<Chunk>::iterator next_chunk = allocated_chunks_[num - 1];
            ++next_chunk;
            if (next_chunk->is_free_) {
                allocated_chunks_[num - 1]->length_ += next_chunk->length_;
                memory_heap_.Remove(next_chunk->index_);
                chunks_list_.erase(next_chunk);
            }
        }

        memory_heap_.Insert(allocated_chunks_[num - 1]);
    }
};

void ReadInputToVector(std::istream& input, vector<int>* vec)
{
    int number_of_numbers, cur_number;
    input >> number_of_numbers;
    
    for (int i = 0; i < number_of_numbers; ++i) 
    {
        input >> cur_number;
        vec->push_back(cur_number);
    }
}

void PerformRequests(const vector<int>& requests, vector<int>* output_vec, Allocator* allocator) {
    for (vector<int>::const_iterator it = requests.begin(); it != requests.end(); ++it) {
        if (*it > 0) {
            output_vec->push_back(allocator->AllocateMemory(*it));
        }
        if (*it < 0) {
            allocator->DeallocateMemory(-(*it));
        }
    }
}

void WriteOutput(std::ostream& output, const vector<int>& vec)
{
    for (vector<int>::const_iterator it = vec.begin(); it != vec.end(); ++it) 
    {
        output << *it << "\n";
    }
}

void W3T3::test(Test* const reference)
{
    if (reference != nullptr)
    {
        reference->open(*this).input("6 8 2 3 -1 3 3 -5 2 2 ").expect("1\n3\n-1\n-1\n1\n-1\n");
    }
}

void W3T3::main(std::istream& input, std::ostream& output)
{
    int size_of_memory;
    std::cin >> size_of_memory;
    Allocator allocator(size_of_memory);
    vector<int> requests;
    ReadInputToVector(input, &requests);
    vector<int> output_vec;
    PerformRequests(requests, &output_vec, &allocator);
    WriteOutput(output, output_vec);

}