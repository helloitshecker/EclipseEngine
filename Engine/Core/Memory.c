#include "Memory.h"

#include <Engine/Core/Debug.h>
#include <Engine/Core/Types.h>
#include <stdlib.h>

EMemory* eMemory_Create(const u64 size) {
        EDEBUG("Allocating %llu memory!", size);
        EMemory* out = (EMemory*)malloc(sizeof(EMemory));
        EASSERT(out);

        out->start = (uintptr)malloc(size);
        EASSERT(out->start);
        out->end = out->start + size;
        out->current = out->start;

        EDEBUG("Allocation Successful!");
        return out;
}

void eMemory_Destroy(EMemory* memory) {
        free((void*)memory->start);
        free(memory);
        EDEBUG("Deallocation Successful!");
}

void* eMemory_Alloc(EMemory* mem, const u64 size) {
        EDEBUG("Allocating %llu memory from pool!", size);
        EASSERT_MSG(mem->end-mem->current > size, "Memory Pool Overflow Error!");

        void* returnPtr = (void*)mem->current;
        mem->current += size;

        EDEBUG("Memory Status: %llu/%llu memory!", mem->end-mem->current, mem->end-mem->start);

        return returnPtr;
}