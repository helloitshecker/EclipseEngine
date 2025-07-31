#pragma once

#include <Engine/Core/Types.h>

typedef struct {
        uintptr start;
        uintptr current;
        uintptr end;
} EMemory;

EMemory* eMemory_Create(const u64 size);
void eMemory_Destroy(EMemory* memory);

void* eMemory_Alloc(EMemory* mem, const u64 size);