#include "FixedSizeAllocator.h"

#include "HeapAllocator.h"
#include <iostream>

FixedSizeAllocator::FixedSizeAllocator(size_t size, size_t numBlocks, uintptr_t startLoc)
{

	_blockSize = size;
	_numBlocks = numBlocks;
	_startLoc = startLoc;

	_freeList = new BitArray(_numBlocks);
	_freeList->setAll();
	_freeList->clearBit(0);
}

FixedSizeAllocator::~FixedSizeAllocator()
{
	delete _freeList;
}

uintptr_t FixedSizeAllocator::getStartLoc()
{
	return _startLoc;
}


void* FixedSizeAllocator::alloc(size_t size)
{
	//std::cout << "size : " << size;

	if (size > _blockSize) return nullptr;
	size_t bitIndex = 0;
	if (!_freeList->getFirstSetBit(bitIndex)) return nullptr;
	_freeList->clearBit(bitIndex);
	return reinterpret_cast<void*>(_startLoc + bitIndex * _blockSize);
}

bool FixedSizeAllocator::free(void* dataPtr)
{

	ptrdiff_t diff = reinterpret_cast<uintptr_t>(dataPtr) - _startLoc;

	if (diff > 0 && diff % _blockSize == 0 && (diff / _blockSize) < _numBlocks)
	{
		ptrdiff_t ptrDiff = reinterpret_cast<uintptr_t>(dataPtr) - _startLoc;
		_freeList->setBit(ptrDiff / _blockSize);

		return true;
	}

	return false;
}

void FixedSizeAllocator::destroy()
{
	_freeList->setAll();
}
