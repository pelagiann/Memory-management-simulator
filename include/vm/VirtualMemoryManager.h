#ifndef VIRTUAL_MEMORY_MANAGER_H
#define VIRTUAL_MEMORY_MANAGER_H

#include "vm/PageTableEntry.h"
#include "MemoryManager.h"
#include "cache/Cache.h"

#include <unordered_map>
#include <queue>
#include <string>
#include <utility>

class VirtualMemoryManager {
private:
    static const size_t PAGE_SIZE = 256;

    MemoryManager& phys_mem;
    Cache& cache;

    size_t max_frames;
    size_t used_frames;
    size_t timestamp;

    std::string replacement_policy;

    // pid -> (vpn -> entry): each process has its own page table
    std::unordered_map<size_t, std::unordered_map<size_t, PageTableEntry>> page_tables;
    std::queue<std::pair<size_t, size_t>> fifo_queue;   // (pid, vpn) in load order

    int allocate_frame();
    int evict_page();

public:
    VirtualMemoryManager(MemoryManager& mm,
                         Cache& l1,
                         size_t total_memory,
                         const std::string& policy);

    void access(size_t pid, size_t virtual_address);
    void print_stats() const;

private:
    size_t page_faults;
    size_t page_evictions;
};

#endif
