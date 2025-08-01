#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Renderer/GL/Driver/RenderDevice.h>
#include <Engine/System/Window.h>

typedef struct {
        EApi api;
        const char* fileName;
} ERenderer_CreateInfo;

ERenderDevice_Layout* eRenderer_Create(ERenderer_CreateInfo* createInfo);
void eRenderer_Destroy(ERenderDevice_Layout* renderDevice);