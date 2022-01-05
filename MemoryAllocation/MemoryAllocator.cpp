#include "MemEngine.h"


void* __cdecl malloc(size_t i_size, HeapAllocator* hm) {
	return hm->alloc(i_size,4);
}

void __cdecl free(void* i_ptr, HeapAllocator* hm) {
	hm->free(i_ptr);
}

void* operator new(size_t i_size) {
	return malloc(i_size, HeapAllocator::hm_instance);
}

void operator delete(void* i_ptr) {
	free(i_ptr, HeapAllocator::hm_instance);
}

void* operator new[](size_t i_size) {
	return malloc(i_size, HeapAllocator::hm_instance);
}

void operator delete [](void* i_ptr) {
	free(i_ptr, HeapAllocator::hm_instance);
}
