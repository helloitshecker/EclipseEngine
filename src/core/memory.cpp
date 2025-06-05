#include "memory.hpp"

#ifdef _WIN32
#include <Windows.h>

void* ee::Memory::Allocate(size_t size) {
	void* ptr = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!ptr) throw MemoryAllocationFailed;
	return ptr;
}

void ee::Memory::Free(void* ptr) {
	if (ptr) {
		VirtualFree(ptr, 0, MEM_RELEASE);
	}
}

#endif