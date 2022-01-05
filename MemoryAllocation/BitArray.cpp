#include "BitArray.h"
#include <cstring>

BitArray::BitArray(size_t num_bits) : bitNumbers(num_bits)
{
	bitData = new t_BitData[(1 + (bitNumbers - 1) / bitsPerElement)];
}

BitArray::~BitArray()
{
	delete[] bitData;
}

void BitArray::setAll()
{
	memset(bitData, ~0, (1 + (bitNumbers - 1) / bitsPerElement) * sizeof(t_BitData));
}

void BitArray::setBit(size_t bitIndex)
{

	t_BitData* dataElem = &bitData[bitIndex / bitsPerElement];
	size_t bitPos = bitIndex % bitsPerElement;

	t_BitData shiftedBits = (static_cast<t_BitData>(1) << (bitsPerElement - bitPos - 1));
	*dataElem |= shiftedBits;
}

void BitArray::clearBit(size_t bitIndex)
{

	t_BitData* dataElem = &bitData[bitIndex / bitsPerElement];
	size_t bitPos = bitIndex % bitsPerElement;

	t_BitData shiftedBits = ~(static_cast<t_BitData>(1) << (bitsPerElement - bitPos - 1));
	*dataElem &= shiftedBits;
}

bool BitArray::getFirstSetBit(size_t& bitIndex) const
{

	size_t size = (1 + (bitNumbers - 1) / bitsPerElement);
	size_t index;

	for (index = 0; index < size; index++)
	{
		if (bitData[index] != 0)
			break;
	}

	if (index == size)
		return false;

	t_BitData data = bitData[index];

	unsigned long pos;
#if defined(WIN32)
	_BitScanReverse(&pos, data);
#else
	_BitScanReverse64(&pos, data);
#endif
	size_t bitPos = bitsPerElement - pos - 1;

	bitIndex = index * bitsPerElement + bitPos;
	return true;
}