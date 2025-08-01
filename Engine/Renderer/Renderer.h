#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Renderer/GL/Driver/RenderDevice.h>
#include <Engine/System/Window.h>

typedef struct {
        EApi api;               ///<Render Backend
        const char* fileName;   ///<File name for Renderer DLL (Default Name: Engine_Renderer_EXT_GL.dll)
} ERenderer_CreateInfo;

/**
 * @brief Load's Renderer DLL and maps all functions to Layout
 * @param createInfo CreateInfo
 * @return Mapped Functions from DLL for rendering
 */
ERenderDevice_Layout* eRenderer_Create(ERenderer_CreateInfo* createInfo);

/**
 * @brief Deallocates DLL and Layout Table
 * @param renderDevice Layout to Deallocate
 */
void eRenderer_Destroy(ERenderDevice_Layout* renderDevice);