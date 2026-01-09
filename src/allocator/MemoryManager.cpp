#include "MemoryManager.h"
#include <iostream>
#include <iomanip>

MemoryManager::MemoryManager()
    : head(nullptr),
      total_memory(0),
      next_block_id(1),
      alloc_requests(0),
      alloc_failures(0) {}

MemoryManager::~MemoryManager() {
    MemoryBlock* curr = head;
    while (curr) {
        MemoryBlock* next = curr->next;
        delete curr;
        curr = next;
    }
}

void MemoryManager::init(size_t size) {
    total_memory = size;
    head = new MemoryBlock(0, size);
}

void MemoryManager::dump() const {
    MemoryBlock* curr = head;
    while (curr) {
        std::cout << "[0x" << std::hex << std::setw(4) << std::setfill('0') << curr->start << " - 0x" 
                  << std::setw(4) << (curr->start + curr->size - 1) << "] ";

        if (curr->free)
            std::cout << "FREE\n";
        else
            std::cout << "USED (id=" << curr->block_id << ")\n";

        curr = curr->next;
    }

    std::cout << std::dec;
}

size_t MemoryManager::align_up(size_t size) const {
    return (size + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
}

MemoryBlock* MemoryManager::split_and_allocate(
    MemoryBlock* block, size_t req_size) {

    // Exact fit
    if (block->size == req_size) {
        block->free = false;
        block->block_id = next_block_id++;
        return block;
    }

    // Split block
    MemoryBlock* new_block =
        new MemoryBlock(block->start + req_size,
                        block->size - req_size);

    new_block->next = block->next;
    new_block->prev = block;

    if (block->next)
        block->next->prev = new_block;

    block->next = new_block;

    block->size = req_size;
    block->free = false;
    block->block_id = next_block_id++;

    return block;
}

int MemoryManager::allocate_first_fit(size_t req_size) {
    alloc_requests++;
    size_t aligned = align_up(req_size);
    MemoryBlock* curr = head;

    while (curr) {
        if (curr->free && curr->size >= aligned) {
            MemoryBlock* block = split_and_allocate(curr, aligned);
            block->requested = req_size;
            return block->block_id;
        }
        curr = curr->next;
    }
    alloc_failures++;
    return -1;
}

int MemoryManager::allocate_best_fit(size_t req_size) {
    alloc_requests++;
    size_t aligned = align_up(req_size);
    MemoryBlock* curr = head;
    MemoryBlock* best = nullptr;
    size_t best_diff = SIZE_MAX;

    while (curr) {
        if (curr->free && curr->size >= aligned) {
            size_t diff = curr->size - aligned;
            if (diff < best_diff) {
                best_diff = diff;
                best = curr;
            }
        }
        curr = curr->next;
    }

    if (!best){
        alloc_failures++;
        return -1;
    }

    MemoryBlock* block = split_and_allocate(best, aligned);
    block->requested = req_size;
    return block->block_id;
}

int MemoryManager::allocate_worst_fit(size_t req_size) {
    alloc_requests++;
    size_t aligned = align_up(req_size);
    MemoryBlock* curr = head;
    MemoryBlock* worst = nullptr;
    size_t worst_size = 0;

    while (curr) {
        if (curr->free && curr->size >= aligned) {
            if (curr->size > worst_size) {
                worst_size = curr->size;
                worst = curr;
            }
        }
        curr = curr->next;
    }

    if (!worst){
        alloc_failures++;
        return -1;
    }

    MemoryBlock* block = split_and_allocate(worst, aligned);
    block->requested = req_size;
    return block->block_id;
}



bool MemoryManager::free_block(int block_id) {
    MemoryBlock* curr = head;
    while (curr) {
        if (!curr->free && curr->block_id == block_id) {

            // Mark block as free
            curr->free = true;
            curr->block_id = -1;

            // Combine with next block if free
            if (curr->next && curr->next->free) {
                MemoryBlock* next = curr->next;
                curr->size += next->size;
                curr->next = next->next;
                if (next->next)
                    next->next->prev = curr;
                delete next;
            }

            // Combine with previous block if free
            if (curr->prev && curr->prev->free) {
                MemoryBlock* prev = curr->prev;
                prev->size += curr->size;
                prev->next = curr->next;
                if (curr->next)
                    curr->next->prev = prev;
                delete curr;
            }

            return true;
        }

        curr = curr->next;
    }

    return false; //block ID not found
}


size_t MemoryManager::total_free_memory() const {
    size_t total = 0;
    MemoryBlock* curr = head;

    while (curr) {
        if (curr->free)
            total += curr->size;
        curr = curr->next;
    }

    return total;
}

size_t MemoryManager::largest_free_block() const {
    size_t largest = 0;
    MemoryBlock* curr = head;

    while (curr) {
        if (curr->free && curr->size > largest)
            largest = curr->size;
        curr = curr->next;
    }

    return largest;
}

double MemoryManager::external_fragmentation() const {
    size_t total_free = total_free_memory();

    if (total_free == 0)
        return 0.0;

    size_t largest = largest_free_block();
    return 1.0 - (double)largest / total_free;
}

double MemoryManager::memory_utilization() const {
    size_t used = 0;
    MemoryBlock* curr = head;

    while (curr) {
        if (!curr->free)
            used += curr->size;
        curr = curr->next;
    }

    return (double)used / total_memory;
}

size_t MemoryManager::get_alloc_requests() const {
    return alloc_requests;
}

size_t MemoryManager::get_alloc_failures() const {
    return alloc_failures;
}

double MemoryManager::allocation_success_rate() const {
    if (alloc_requests == 0) return 0.0;
    return 1.0 - (double)alloc_failures / alloc_requests;
}

double MemoryManager::allocation_failure_rate() const {
    if (alloc_requests == 0) return 0.0;
    return (double)alloc_failures / alloc_requests;
}

size_t MemoryManager::internal_fragmentation() const {
    size_t wasted = 0;
    MemoryBlock* curr = head;

    while (curr) {
        if (!curr->free)
            wasted += curr->size - curr->requested;
        curr = curr->next;
    }

    return wasted;
}

size_t MemoryManager::get_block_start(int block_id) const {
    MemoryBlock* curr = head;

    while (curr) {
        if (!curr->free && curr->block_id == block_id) {
            return curr->start;
        }
        curr = curr->next;
    }

    return static_cast<size_t>(-1);
}


void MemoryManager::print_stats() const {
    std::cout << "--- Memory Stats ---\n";
    std::cout << "Total free memory: " << total_free_memory() << "\n";
    std::cout << "Largest free block: " << largest_free_block() << "\n";
    std::cout << "Memory utilization: " << memory_utilization() << "\n";
    std::cout << "Allocation requests: " << get_alloc_requests() << "\n";
    std::cout << "Allocation failures: " << get_alloc_failures() << "\n";
    std::cout << "Allocation success rate: "<< allocation_success_rate() *100 << "%\n";
    std::cout << "Allocation failure rate: "<< allocation_failure_rate() * 100 << "%\n";
    std::cout << "Internal fragmentation: " << internal_fragmentation() << " bytes\n";
    std::cout << "External fragmentation: " << external_fragmentation() << "\n";
}

