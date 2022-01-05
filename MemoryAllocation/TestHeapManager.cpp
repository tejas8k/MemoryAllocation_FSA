#include <Windows.h>

#include "MemEngine.h"

#include <assert.h>
#include <algorithm>
#include <vector>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#endif // _DEBUG

bool MemorySystem_UnitTest(HeapAllocator*);

void FinalHeapTest() {
	const size_t sizeHeap = 1024 * 1024;

	// you may not need this if you don't use a descriptor pool
	const unsigned int 	numDescriptors = 2048;

	// Allocate memory for my test heap.
	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	assert(pHeapMemory);

	// Create your HeapAllocator and FixedSizeAllocators.
	auto heapAllocator = InitMemoryEngine(pHeapMemory, sizeHeap, numDescriptors);

	// return;
	bool success = MemorySystem_UnitTest(heapAllocator);
	assert(success);

	// Destroy HeapAllocator and FixedSizeAllocators
	DestroyMemEngine(heapAllocator);

#ifdef _DEBUG
	heapAllocator->DisplayOutstandingAllocations();
#endif
	HeapFree(GetProcessHeap(), 0, pHeapMemory);

	// check for memory leaks
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
}

bool MemorySystem_UnitTest(HeapAllocator* heapAllocator) {
	const size_t maxAllocations = 1024 * 10;
	std::vector<void*> AllocatedAddresses;

	long	numAllocs = 0;
	long	numFrees = 0;
	long	numCollects = 0;

	size_t totalAllocated = 0;

	// reserve space in AllocatedAddresses for the maximum number of allocation attempts
	// prevents new returning null when std::vector expands the underlying array
	AllocatedAddresses.reserve(maxAllocations);

	// return true;
	// allocate memory of random sizes up to 1024 bytes from the heap manager
	// until it runs out of memory
	do {
		const size_t		maxTestAllocationSize = 1024;

		size_t			sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));

		void* pPtr = malloc(sizeAlloc, heapAllocator);

		// if allocation failed see if garbage collecting will create a large enough block
		if (pPtr == nullptr) {
			Collect(heapAllocator);

			pPtr = malloc(sizeAlloc, heapAllocator);

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

			free(pPtrToFree, heapAllocator);
			numFrees++;
		}
		else if ((rand() % garbageCollectAboutEvery) == 0) {
			Collect(heapAllocator);

			numCollects++;
		}

	} while (numAllocs < maxAllocations);

	// now free those blocks in a random order
	if (!AllocatedAddresses.empty()) {
		// randomize the addresses
		std::random_shuffle(AllocatedAddresses.begin(), AllocatedAddresses.end());

		// return them back to the heap manager
		while (!AllocatedAddresses.empty()) {
			void* pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			delete pPtrToFree;
		}

		Collect(heapAllocator);

#ifdef _DEBUG 
		std::cout << "Printing outstanding allocations after GC" << std::endl;
		heapAllocator->DisplayOutstandingAllocations();
#endif

		// do a large test allocation to see if garbage collection worked
		void* pPtr = malloc(totalAllocated / 2, heapAllocator);

		if (pPtr) {
			free(pPtr, heapAllocator);
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

	// we succeeded
	return true;
}
