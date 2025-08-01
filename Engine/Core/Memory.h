#pragma once

#include <Engine/Core/Types.h>

/**
 * start: pointer to start of allocated memory
 * current: pointer to unused memory but allocated
 * end: pointer to the end of allocated memory
 */
typedef struct {
        uintptr start;
        uintptr current;
        uintptr end;
} EMemory;

/**
 * @brief Pre-Allocates Memory of given size.
 * @param size Amount of memory to allocate in bits.
 * @return EMemory* if successful else terminates program.
 */
EMemory* eMemory_Create(const u64 size);

/**
 * @brief De-allocates Memory
 * @param memory Pointer to EMemory* struct.
 */
void eMemory_Destroy(EMemory* memory);

/**
 * @brief Assigns Memory by-request from Pre-allocated memory
 * @param mem Memory Pointer
 * @param size Amount of memory requested in bits.
 * @return pointer to requested memory.
 */
void* eMemory_Alloc(EMemory* mem, const u64 size);