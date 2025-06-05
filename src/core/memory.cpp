#include "memory.hpp"

constexpr size_t MAX_ALLOCATIONS = 8096;

#ifdef _WIN32
#include <Windows.h>

void* allocation_table[MAX_ALLOCATIONS];
size_t allocation_pointer{ 0 };

void* ee::Memory::Allocate(size_t size) {
	void* ptr = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!ptr) throw MemoryAllocationFailed;

	allocation_table[allocation_pointer] = ptr;
	allocation_pointer++;

	return ptr;
}

void ee::Memory::Free(void* ptr) {
	if (ptr) {
		VirtualFree(ptr, 0, MEM_RELEASE);
	}
}

void ee::Memory::FreeAll() {
	for (; allocation_pointer > 0; allocation_pointer--) {
		Free(allocation_table[allocation_pointer]);
	}
}

#endif