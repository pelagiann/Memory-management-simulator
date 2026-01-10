# Memory Management Simulator
A comprehensive user-space simulator that models how operating systems manage physical memory, CPU caches and virtual memory. This educational tool demonstrates core OS memory management concepts through algorithmic correctness, realistic behavior, and clean modular design.

# Video
This video showcases the various functions that have been incorporated in this simulator
```bash
https://drive.google.com/file/d/1uHfcS9eK97FwVZj5_RyZQlIQUtZzNGV2/view?usp=sharing
```

# Overview
The simulator implements a complete memory management pipeline:
Virtual Address → Page Table → Physical Address → Cache (L1 → L2) → Physical Memory  
This is a faithful conceptual simulation implemented in user space: not a real OS kernel, but an accurate representation of how these systems work.
# Core Components
## Physical Memory Management

1. Contiguous memory simulation with dynamic allocation/deallocation  
2. Block splitting and coalescing  
3. Three allocation strategies: First Fit, Best Fit, and Worst Fit  
4. Explicit tracking of free and allocated blocks  

## Cache Hierarchy

1. Multilevel cache system (L1 and L2)  
2. Set-associative cache design  
3. FIFO and LRU replacement policies  
4. Hit/miss tracking with Average Memory Access Time (AMAT) calculation  
5. Miss penalty propagation between cache levels  

## Virtual Memory

1. Paging-based virtual memory with 256-byte pages 
2. Page tables with valid bit tracking 
3. FIFO and LRU page replacement algorithms 
4. Page fault and eviction monitoring 
5. Physical address translation before cache access 

## Statistics & Analysis

1. External fragmentation calculation 
2. Allocation success/failure rates 
3. Memory utilization metrics 
4. Cache performance analysis 
5. Page fault tracking 

# Getting Started
## Requirements

C++17 compatible compiler (tested with g++)

## Building
```bash
g++ -std=c++17 -Iinclude src/main.cpp src/allocator/*.cpp src/cache/*.cpp src/vm/*.cpp src/buddy/*.cpp -o memory_sim
```
## Running
Interactive mode:  
```bash
./memory_sim  
  ```
Scripted workloads(Example):  
```bash
./memory_sim < tests/allocator_basic.txt  
  ```
# Command Reference

**`init <size>`**  
Initialize physical memory with the specified size in bytes.
```bash
init 1024
```

**`alloc <strategy> <size>`**  
Allocate memory using the specified strategy (first/best/worst).
```bash
alloc first 100
```

**`free <block_id>`**  
Deallocate the memory block with the given ID.
```bash
free 3
```

**`access <virtual_addr>`**  
Access a virtual memory address (triggers address translation, cache lookup, potential page faults).
```bash
access 512
```

**`dump`**  
Display the current state of memory (all allocated and free blocks).

**`stats`**  
Show allocation statistics including success/failure rates and fragmentation.

**`cache_stats`**  
Display cache performance metrics (hits, misses, hit rate, AMAT).

**`vm_stats`**  
Show virtual memory statistics (page faults, evictions, resident pages).

**`help`**  
Display all available commands.

# Project Structure
```
memory-simulator/
├── src/
│   ├── allocator/     # Memory allocation algorithms
│   ├── cache/         # Cache hierarchy implementation
│   ├── vm/            # Virtual memory system
│   └── main.cpp       # CLI and main loop
├── include/           # Header files
├── tests/             # Scripted workload files
├── logs/              # Outputs of the tests
├── DOCUMENTATION.md   # Detailed design documentation
└── README.md

```
# Testing
The tests/ directory contains scripted workloads demonstrating:

1. Memory allocation and deallocation patterns  
2. Fragmentation behavior across strategies  
3. Cache locality and conflict scenarios  
4. Page fault triggering and replacement  
5. End-to-end system integration  
