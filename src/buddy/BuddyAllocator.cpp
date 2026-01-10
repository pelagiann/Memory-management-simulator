#include "buddy/BuddyAllocator.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

BuddyAllocator::BuddyAllocator()
    : total_size(0),
      max_order(0),
      next_block_id(1),
      alloc_requests(0),
      alloc_failures(0) {}

size_t BuddyAllocator::order_for(size_t size) const {
    if (size == 0) size = 1;
    size_t order = 0;
    while ((size_t(1) << order) < size)
        order++;
    return order;
}

// Size must be a power of two
bool BuddyAllocator::init(size_t size) {
    if (size == 0 || (size & (size - 1)) != 0)
        return false;

    total_size = size;
    max_order = order_for(size);

    free_lists.assign(max_order + 1, {});
    allocated.clear();
    free_lists[max_order].push_back(0);

    next_block_id = 1;
    alloc_requests = 0;
    alloc_failures = 0;
    return true;
}

int BuddyAllocator::allocate(size_t size) {
    alloc_requests++;
    size_t need = order_for(size);

    // Find the smallest available block that can hold the request
    size_t order = need;
    while (order <= max_order && free_lists[order].empty())
        order++;

    if (order > max_order) {
        alloc_failures++;
        return -1;
    }

    size_t start = free_lists[order].back();
    free_lists[order].pop_back();

    // Split down to the needed order, freeing each buddy
    while (order > need) {
        order--;
        size_t buddy = start + (size_t(1) << order);
        free_lists[order].push_back(buddy);
    }

    int id = next_block_id++;
    allocated[id] = { start, need, size };
    return id;
}

bool BuddyAllocator::remove_from_free(size_t order, size_t start) {
    auto& fl = free_lists[order];
    for (size_t i = 0; i < fl.size(); ++i) {
        if (fl[i] == start) {
            fl[i] = fl.back();
            fl.pop_back();
            return true;
        }
    }
    return false;
}

bool BuddyAllocator::free_block(int block_id) {
    auto it = allocated.find(block_id);
    if (it == allocated.end())
        return false;

    size_t start = it->second.start;
    size_t order = it->second.order;
    allocated.erase(it);

    // Coalesce with buddy while it is also free
    while (order < max_order) {
        size_t buddy = start ^ (size_t(1) << order);
        if (!remove_from_free(order, buddy))
            break;
        if (buddy < start)
            start = buddy;
        order++;
    }

    free_lists[order].push_back(start);
    return true;
}

void BuddyAllocator::dump() const {
    std::vector<std::pair<size_t, int>> blocks;   // start -> id
    for (auto& e : allocated)
        blocks.push_back({ e.second.start, e.first });
    std::sort(blocks.begin(), blocks.end());

    std::cout << "Allocated blocks:\n";
    for (auto& b : blocks) {
        const Allocation& a = allocated.at(b.second);
        size_t bsize = size_t(1) << a.order;
        std::cout << "  [0x" << std::hex << std::setw(4) << std::setfill('0') << a.start
                  << " - 0x" << std::setw(4) << (a.start + bsize - 1) << "] "
                  << std::dec << "id=" << b.second
                  << " size=" << bsize << " requested=" << a.requested << "\n";
    }

    std::cout << "Free blocks by order:\n";
    for (size_t order = 0; order <= max_order; ++order) {
        if (free_lists[order].empty())
            continue;
        std::cout << "  order " << order << " (" << (size_t(1) << order) << " bytes):";
        for (size_t start : free_lists[order])
            std::cout << " 0x" << std::hex << std::setw(4) << std::setfill('0') << start << std::dec;
        std::cout << "\n";
    }
}

void BuddyAllocator::print_stats() const {
    size_t used = 0;
    size_t internal = 0;

    for (auto& e : allocated) {
        size_t bsize = size_t(1) << e.second.order;
        used += bsize;
        internal += bsize - e.second.requested;
    }

    std::cout << "--- Buddy Stats ---\n";
    std::cout << "Total memory: " << total_size << "\n";
    std::cout << "Used memory: " << used << "\n";
    std::cout << "Free memory: " << (total_size - used) << "\n";
    std::cout << "Memory utilization: " << (double)used / total_size << "\n";
    std::cout << "Internal fragmentation: " << internal << " bytes\n";
    std::cout << "Allocation requests: " << alloc_requests << "\n";
    std::cout << "Allocation failures: " << alloc_failures << "\n";
}
