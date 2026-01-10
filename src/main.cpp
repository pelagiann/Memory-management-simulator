#include "MemoryManager.h"
#include "cache/Cache.h"
#include "vm/VirtualMemoryManager.h"
#include "buddy/BuddyAllocator.h"
#include <iostream>
#include <sstream>
#include <string>


int main() {
    MemoryManager mm;
    BuddyAllocator buddy;
    bool initialized = false;
    bool buddy_initialized = false;

    std::string line;

    std::cout << "Memory Management Simulator\n";
    std::cout << "Type 'help' to see available commands\n";
    std::cout << "Type 'exit' to quit\n";


Cache L2(1024, 64, 4, "LRU"); // 1KB, 4-way
Cache L1(256, 64, 2, "LRU");  // 256B, 2-way
VirtualMemoryManager vmm(mm, L1, 1024, "LRU");

L1.set_next_level(&L2);




    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            break;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "exit") {
            break;
        }

        else if (cmd == "help") {
            std::cout << "Available commands:\n";
            std::cout << "  init <size>                  Initialize memory\n";
            std::cout << "  alloc <first|best|worst> <size>  Allocate memory\n";
            std::cout << "  free <block_id>              Free allocated block\n";
            std::cout << "  dump                          Show memory layout\n";
            std::cout << "  stats                         Show memory statistics\n";
            std::cout << "  access <address>              Access memory address via cache\n";
            std::cout << "  cache_stats                  Show cache statistics\n";
            std::cout << "  vm_stats                     Show virtual memory statistics\n";
            std::cout << "  buddy_init <size>            Initialize buddy allocator (power of two)\n";
            std::cout << "  buddy_alloc <size>           Allocate via buddy system\n";
            std::cout << "  buddy_free <block_id>        Free a buddy block\n";
            std::cout << "  buddy_dump                   Show buddy memory layout\n";
            std::cout << "  buddy_stats                  Show buddy statistics\n";
            std::cout << "  exit                          Exit simulator\n";

        }


        else if (cmd == "init") {
            size_t size;
            ss >> size;

            if (!ss || size == 0) {
                std::cout << "Usage: init <size>\n";
                continue;
            }

            mm.init(size);
            initialized = true;
            std::cout << "Initialized memory with size " << size << "\n";
        }

        else if (cmd == "alloc") {
            if (!initialized) {
                std::cout << "Memory not initialized\n";
                continue;
            }

            std::string strategy;
            size_t size;
            ss >> strategy >> size;

            if (!ss) {
                std::cout << "Usage: alloc <first|best|worst> <size>\n";
                continue;
            }

            int id = -1;

            if (strategy == "first")
                id = mm.allocate_first_fit(size);
            else if (strategy == "best")
                id = mm.allocate_best_fit(size);
            else if (strategy == "worst")
                id = mm.allocate_worst_fit(size);
            else {
                std::cout << "Unknown strategy\n";
                continue;
            }

            if (id == -1)
                std::cout << "Allocation failed\n";
            else
                std::cout << "Allocated block id " << id << "\n";
        }

        else if (cmd == "free") {
            if (!initialized) {
                std::cout << "Memory not initialized\n";
                continue;
            }

            int id;
            ss >> id;

            if (!ss) {
                std::cout << "Usage: free <block_id>\n";
                continue;
            }

            if (mm.free_block(id))
                std::cout << "Freed block " << id << "\n";
            else
                std::cout << "Invalid block id\n";
        }

        else if (cmd == "dump") {
            if (!initialized) {
                std::cout << "Memory not initialized\n";
                continue;
            }

            mm.dump();
        }

        else if (cmd == "stats") {
            if (!initialized) {
                std::cout << "Memory not initialized\n";
                continue;
            }

            mm.print_stats();
        }

        else if (cmd == "access") {
            size_t address;
            ss >> address;

            if (!ss) {
                std::cout << "Usage: access <virtual_address>\n";
                continue;
            }

            vmm.access(address);
        }


        else if (cmd == "cache_stats") {
            L1.print_stats("L1");
            L2.print_stats("L2");
        }

        else if (cmd == "vm_stats") {
            vmm.print_stats();
        }

        else if (cmd == "buddy_init") {
            size_t size;
            ss >> size;

            if (!ss || size == 0) {
                std::cout << "Usage: buddy_init <size>\n";
                continue;
            }

            if (!buddy.init(size)) {
                std::cout << "Size must be a power of two\n";
                continue;
            }

            buddy_initialized = true;
            std::cout << "Initialized buddy allocator with size " << size << "\n";
        }

        else if (cmd == "buddy_alloc") {
            if (!buddy_initialized) {
                std::cout << "Buddy allocator not initialized\n";
                continue;
            }

            size_t size;
            ss >> size;

            if (!ss) {
                std::cout << "Usage: buddy_alloc <size>\n";
                continue;
            }

            int id = buddy.allocate(size);

            if (id == -1)
                std::cout << "Allocation failed\n";
            else
                std::cout << "Allocated block id " << id << "\n";
        }

        else if (cmd == "buddy_free") {
            if (!buddy_initialized) {
                std::cout << "Buddy allocator not initialized\n";
                continue;
            }

            int id;
            ss >> id;

            if (!ss) {
                std::cout << "Usage: buddy_free <block_id>\n";
                continue;
            }

            if (buddy.free_block(id))
                std::cout << "Freed block " << id << "\n";
            else
                std::cout << "Invalid block id\n";
        }

        else if (cmd == "buddy_dump") {
            if (!buddy_initialized) {
                std::cout << "Buddy allocator not initialized\n";
                continue;
            }

            buddy.dump();
        }

        else if (cmd == "buddy_stats") {
            if (!buddy_initialized) {
                std::cout << "Buddy allocator not initialized\n";
                continue;
            }

            buddy.print_stats();
        }

        else {
            std::cout << "Unknown command\n";
        }
    }

    return 0;
}
