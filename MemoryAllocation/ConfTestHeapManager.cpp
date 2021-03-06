#include <Windows.h>

#include "MemEngine.h"
#include "MemoryAllocator.h"

#include <assert.h>
#include <algorithm>
#include <vector>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#endif // _DEBUG

bool MemorySystem_UnitTest(HeapManager*);

void HeapTestFinal() {
	const size_t 		sizeHeap = 1024 * 1024;

	// you may not need this if you don't use a descriptor pool
	const unsigned int 	numDescriptors = 2048;

	// Allocate memory for my test heap.
	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	assert(pHeapMemory);

	// Create your HeapManager and FixedSizeAllocators.
	auto hm = InitMemoryEngine(pHeapMemory, sizeHeap, numDescriptors);

#if _DEBUG
	std::cout << "\n\n Memory System Initialized!!! " << std::endl;
	hm->DisplayOutstandingAllocations();
#endif
	// return;
	bool success = MemorySystem_UnitTest(hm);
	assert(success);

#if _DEBUG
	std::cout << "\n\nMemory System Unit Test Completed!!! " << std::endl;
	hm->DisplayOutstandingAllocations();
#endif

	// Clean up your Memory System (HeapManager and FixedSizeAllocators)
	DestroyMemEngine(hm);

#if _DEBUG
	std::cout << "\n\nMemory System Destroyed!!! " << std::endl;
	hm->DisplayOutstandingAllocations();
#endif

	HeapFree(GetProcessHeap(), 0, pHeapMemory);

	// in a Debug build make sure we didn't leak any memory.
#if defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif // _DEBUG
}

bool MemorySystem_UnitTest(HeapManager* hm) {
	const size_t maxAllocations = 1024 * 10;
	std::vector<void*> AllocatedAddresses;

	long	numAllocs = 0;
	long	numFrees = 0;
	long	numCollects = 0;

	size_t totalAllocated = 0;

	// reserve space in AllocatedAddresses for the maximum number of allocation attempts
	// prevents new returning null when std::vector expands the underlying array
	AllocatedAddresses.reserve(10 * 1024);

	// return true;
	// allocate memory of random sizes up to 1024 bytes from the heap manager
	// until it runs out of memory
	do {
		const size_t		maxTestAllocationSize = 1024;

		size_t			sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));

		void* pPtr = malloc(sizeAlloc, hm);

		// if allocation failed see if garbage collecting will create a large enough block
		if (pPtr == nullptr) {
			Collect(hm);

			pPtr = malloc(sizeAlloc, hm);

			// if not we're done. go on to cleanup phase of test
			if (pPtr == nullptr)
				break;
		}

		AllocatedAddresses.push_back(pPtr);
		numAllocs++;

		totalAllocated += sizeAlloc;

		// randomly free and/or garbage collect during allocation phase
		const unsigned int freeAboutEvery = 0x07;
		const unsigned int garbageCollectAboutEvery = 0x07;

		if (!AllocatedAddresses.empty() && ((rand() % freeAboutEvery) == 0)) {
			void* pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			free(pPtrToFree, hm);
			numFrees++;
		}
		else if ((rand() % garbageCollectAboutEvery) == 0) {
			Collect(hm);

			numCollects++;
		}

	} while (numAllocs < maxAllocations);

#if _DEBUG
	std::cout << "\n\nAllocation exhausted!! " << std::endl;
	hm->showFreeBlocks();
#endif
	// now free those blocks in a random order
	if (!AllocatedAddresses.empty()) {
		// randomize the addresses
		std::random_shuffle(AllocatedAddresses.begin(), AllocatedAddresses.end());

		// return them back to the heap manager
		while (!AllocatedAddresses.empty()) {
			void* pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			delete[] pPtrToFree;
		}

#if _DEBUG
		std::cout << "\n\nFreed all allocations " << std::endl;
		hm->DisplayOutstandingAllocations();
#endif
		// do garbage collection
		Collect(hm);
		// our heap should be one single block, all the memory it started with
#if _DEBUG
		std::cout << "\n\nGarbage collection completed " << std::endl;
		hm->showFreeBlocks();
#endif
		// do a large test allocation to see if garbage collection worked
		void* pPtr = malloc(totalAllocated / 2, hm);

		if (pPtr) {
			free(pPtr, hm);
		}
		else {
			// something failed
			return false;
		}
	}
	else {
		return false;
	}

	// this new [] / delete [] pair should run through your allocator
	char* pNewTest = new char[1024];

	delete[] pNewTest;

	hm->showFreeBlocks();
	hm->DisplayOutstandingAllocations();

	// we succeeded
	return true;
}
