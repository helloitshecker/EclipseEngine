#include "Memory.h"

#include <Engine/Core/Debug.h>
#include <Engine/Core/Types.h>
#include <stdlib.h>

EMemory* eMemory_Create(const u64 size) {
        EDEBUG("Allocating %llu memory!", size);
        EMemory* out = (EMemory*)malloc(sizeof(EMemory));
        if (out == nullptr) {
                EERROR("Failed to allocate memory!");
                return nullptr;
        }

        out->start = (uintptr)malloc(size);
        if (out->start == 0) {
                EERROR("Failed to allocate memory!");
                free(out);
                return nullptr;
        }
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
        EDEBUG("%llu bytes requested from pool!", size);
        if (mem->end-mem->current < size) {
                EERROR("Memory allocation failed!");
                return nullptr;
        }

        void* returnPtr = (void*)mem->current;
        mem->current += size;

        EDEBUG("Memory Status: Used %llu/%llu bytes", mem->current-mem->start, mem->end-mem->start);

        return returnPtr;
}