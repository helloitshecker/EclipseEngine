#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/Core/Events.h>

typedef struct {
        void* ptr;
} EWindow;

typedef struct {
        char* title;
        int width;
        int height;
        bool resizable;
        bool fullscreen;
        EApi render_api;
        EMemory* memory;
        EEvents* events;
} EWindow_CreateInfo;

EWindow* eWindow_Create(EWindow_CreateInfo* info);
void eWindow_Destroy(EWindow* window);

void eWindow_PollEvent(EWindow* window, EEvents* events);