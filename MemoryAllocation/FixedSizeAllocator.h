#pragma once
#include "BitArray.h"

class FixedSizeAllocator {
public:
	FixedSizeAllocator(size_t size, size_t numBlocks, uintptr_t startLoc);
	~FixedSizeAllocator();
	void* alloc(size_t size);
	bool free(void* dataPtr);
	void destroy();
	uintptr_t getStartLoc();
private:
	// variables of BitArray
	size_t _blockSize, _numBlocks;
	BitArray* _freeList;
	uintptr_t _startLoc;
};