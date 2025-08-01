#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/Memory.h>

EString* eFileSystem_ReadFile(EMemory* memory, const char* filepath, bool binary);