#include "MemEngine.h"

#include "FixedSizeAllocator.h"
#include "HeapAllocator.h"

HeapAllocator* InitMemoryEngine(void* i_pHeapMem, size_t i_sizeHeapMem, unsigned int i_OptionalNumDescriptors)
{

	//Creating HeapManager
	HeapAllocator* hm = CreateHeapAllocator(i_pHeapMem, i_sizeHeapMem);
	size_t blockSizes[3] = { 16, 32, 96 };
	size_t blockCounts[3] = { 100, 200, 400 };

	hm->init_FSA(blockSizes, blockCounts, 3);
	return hm;
}

void Collect(HeapAllocator* hm)
{
	hm->coalesce();
}

void DestroyMemEngine(HeapAllocator* hm)
{
	hm->destroy(); // Destroy HeapManager and FixedSizeAllocators
}