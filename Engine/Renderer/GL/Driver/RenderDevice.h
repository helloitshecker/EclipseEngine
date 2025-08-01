// Driver for OpenGL (Eclipse Engine)
#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/Core/Debug.h>
#include <Engine/System/Window.h>
#include "../glad/include/glad/glad.h"

#ifdef _WIN32
#define EEXPORT __declspec(dllexport)
#define EIMPORT __declspec(dllimport)
#else
#define EEXPORT __attribute__((visibility("default")))
#define EIMPORT
#endif

typedef struct {
        bool (*CreateDevice)(EWindow* window);
        void (*DestroyDevice)();
        void (*Clear)(u8 r, u8 g, u8 b, u8 a);
        void (*SetViewport)(i32 x, i32 y, i32 width, i32 height);
        void (*SetVsync)(bool on);
} ERenderDevice_Layout;

typedef ERenderDevice_Layout* (*PFN_GetRenderDeviceLayout)(void);