#include "Renderer.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
thread_local HMODULE dll;
#elif defined(__linux__)
#else defined(__APPLE__)
#endif

ERenderDevice_Layout* eRenderer_Create(ERenderer_CreateInfo* createInfo) {
#ifdef _WIN32
        dll = LoadLibraryA(createInfo->fileName);
#elif defined(__linux__)
#else defined(__APPLE__)
#endif
        if (!dll) {
                EERROR("Failed to load renderer library from '%s'!", createInfo->fileName);
                return nullptr;
        }

#ifdef _WIN32
        PFN_GetRenderDeviceLayout getLayout = (PFN_GetRenderDeviceLayout)GetProcAddress(dll, "ERenderDevice_GetLayout");
#elif defined(__linux__)
#else defined(__APPLE__)
#endif
        if (!getLayout) {
                EERROR("Failed to get render device layout function!");
#ifdef _WIN32
                FreeLibrary(dll);
#elif defined(__linux__)
#else defined(__APPLE__)
#endif
                return nullptr;
        }

        return getLayout();
}

void eRenderer_Destroy(ERenderDevice_Layout* renderDevice) {
        EDEBUG("Destroying render device!");
#ifdef _WIN32
        FreeLibrary(dll);
#elif defined(__linux__)
#else defined(__APPLE__)
#endif
        renderDevice = nullptr;
}