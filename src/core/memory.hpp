#pragma once
#include <stdexcept>

inline const std::runtime_error MemoryAllocationFailed("Failed to allocate memory!");

namespace ee {
	namespace Memory {
		void* Allocate(size_t size);
		void Free(void* memory);
	};
};