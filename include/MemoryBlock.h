#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

#include <cstddef>

struct MemoryBlock {
	size_t start;
	size_t size;
	size_t requested;   // bytes actually requested (size - requested = internal frag)
	bool free;
	int block_id;

	MemoryBlock* next;
	MemoryBlock* prev;

	MemoryBlock(size_t s, size_t sz);
};

#endif
