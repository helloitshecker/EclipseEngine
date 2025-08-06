#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Renderer/GL/Driver/RenderDevice.h>
#include <Engine/System/Window.h>

/**
 * @brief Load's Renderer DLL and maps all functions to Layout
 * @param api Api request for renderer
 * @return Mapped Functions from DLL for rendering
 */
ERenderDevice_Layout* eRenderer_Create(EApi api);

/**
 * @brief Deallocates DLL and Layout Table
 * @param renderDevice Layout to Deallocate
 */
void eRenderer_Destroy(ERenderDevice_Layout* renderDevice);