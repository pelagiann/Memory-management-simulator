#include "MemoryBlock.h"

MemoryBlock::MemoryBlock(size_t s, size_t sz)
	: start(s), size(sz), requested(0), free(true), block_id(-1),
	next(nullptr), prev(nullptr){}