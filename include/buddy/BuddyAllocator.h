#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <cstddef>
#include <vector>
#include <unordered_map>

class BuddyAllocator {
private:
    struct Allocation {
        size_t start;
        size_t order;
        size_t requested;
    };

    size_t total_size;
    size_t max_order;
    int next_block_id;
    size_t alloc_requests;
    size_t alloc_failures;

    std::vector<std::vector<size_t>> free_lists;    // free_lists[order] = free block starts
    std::unordered_map<int, Allocation> allocated;  // block_id -> allocation

    size_t order_for(size_t size) const;
    bool remove_from_free(size_t order, size_t start);

public:
    BuddyAllocator();

    bool init(size_t size);
    int allocate(size_t size);
    bool free_block(int block_id);
    void dump() const;
    void print_stats() const;
};

#endif
