#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/Memory.h>

/**
 * @brief Reads a file from a specified path and loads it in memory
 * @param memory Memory Pointer for allocating memory to file content
 * @param filepath Path of a file
 * @param binary Read a file as binary or text
 * @return Pointer to EString with file content
 */
EString* eFileSystem_ReadFile(EMemory* memory, const char* filepath, bool binary);