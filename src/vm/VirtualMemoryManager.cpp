#include "vm/VirtualMemoryManager.h"
#include <iostream>

VirtualMemoryManager::VirtualMemoryManager(
    MemoryManager& mm,
    Cache& l1,
    size_t total_memory,
    const std::string& policy)
    : phys_mem(mm),
      cache(l1),
      timestamp(0),
      replacement_policy(policy),
      page_faults(0),
      page_evictions(0),
      used_frames(0) {

    max_frames = total_memory / PAGE_SIZE;
}

int VirtualMemoryManager::allocate_frame() {
    int block_id = phys_mem.allocate_first_fit(PAGE_SIZE);
    if (block_id != -1) {
        used_frames++;
    }
    return block_id;
}

int VirtualMemoryManager::evict_page() {
    size_t victim_pid = 0;
    size_t victim_vpn = 0;

    if (replacement_policy == "FIFO") {
        victim_pid = fifo_queue.front().first;
        victim_vpn = fifo_queue.front().second;
        fifo_queue.pop();
    } else {
        size_t oldest_time = SIZE_MAX;

        // Replacement is global: scan every process's resident pages
        for (auto& proc : page_tables) {
            for (auto& entry : proc.second) {
                PageTableEntry& pte = entry.second;

                if (pte.valid && pte.last_used < oldest_time) {
                    oldest_time = pte.last_used;
                    victim_pid = proc.first;
                    victim_vpn = entry.first;
                }
            }
        }
    }

    PageTableEntry& victim = page_tables[victim_pid][victim_vpn];
    int block_id = victim.block_id;

    phys_mem.free_block(block_id);

    victim.valid = false;
    used_frames--;
    page_evictions++;

    return block_id;
}

void VirtualMemoryManager::access(size_t pid, size_t virtual_address) {
    timestamp++;

    size_t vpn = virtual_address / PAGE_SIZE;
    size_t offset = virtual_address % PAGE_SIZE;

    auto& pte = page_tables[pid][vpn];

    if (pte.valid) {
        pte.last_used = timestamp;

        size_t phys_addr =
            phys_mem.get_block_start(pte.block_id) + offset;
        cache.access(phys_addr);
        return;
    }

    page_faults++;
    std::cout << "[PAGE FAULT] Process " << pid << " virtual page " << vpn << "\n";

    int block_id;

    if (used_frames < max_frames) {
        block_id = allocate_frame();
    } else {
        evict_page();
        block_id = allocate_frame();
    }

    pte.block_id = block_id;
    pte.valid = true;
    pte.loaded_at = timestamp;
    pte.last_used = timestamp;

    if (replacement_policy == "FIFO") {
        fifo_queue.push({ pid, vpn });
    }

    size_t phys_addr =
        phys_mem.get_block_start(block_id) + offset;
   // std::cout << "Phys addr: " << phys_addr << "\n";

    cache.access(phys_addr);
}

void VirtualMemoryManager::print_stats() const {
    std::cout << "--- Virtual Memory Stats ---\n";
    std::cout << "Page faults: " << page_faults << "\n";
    std::cout << "Page evictions: " << page_evictions << "\n";
    std::cout << "Resident pages: " << used_frames << "\n";
}
