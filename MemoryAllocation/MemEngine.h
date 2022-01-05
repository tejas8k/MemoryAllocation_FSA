#pragma once
#include "HeapAllocator.h"

void* malloc(size_t i_size, HeapAllocator* hm);
void free(void* i_ptr, HeapAllocator* hm);


// InitializeMemorySystem - initialize your memory system including your HeapManager and some FixedSizeAllocators
HeapAllocator* InitMemoryEngine(void* i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors);

// Collect - coalesce free blocks in attempt to create larger blocks
void Collect(HeapAllocator* hm);

// DestroyMemorySystem - destroy your memory systems
void DestroyMemEngine(HeapAllocator* hm);
