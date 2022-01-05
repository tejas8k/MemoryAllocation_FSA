#pragma once
#include <cstdint>
#include "intrin.h"

#pragma intrinsic(_BitScanReverse, _BitScanReverse64)

#if defined(WIN32)
typedef uint32_t t_BitData;
#else
typedef uint64_t t_BitData;
#endif

class BitArray
{
public:
	BitArray(size_t num_bits);
	~BitArray();

	void setAll(); //set bits

	void setBit(size_t bitIndex);
	void clearBit(size_t bitIndex);

	bool getFirstSetBit(size_t& bitIndex) const;

private:
	t_BitData* bitData;
	size_t bitNumbers;
	constexpr static size_t bitsPerElement = sizeof(t_BitData) * 8;
};

