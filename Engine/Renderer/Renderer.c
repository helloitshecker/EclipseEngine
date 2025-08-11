#include "Renderer.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif

#ifdef _WIN32
static HMODULE dll;
#elif defined(__linux__)
static void* dll;
#endif

ERenderDevice_Layout* eRenderer_Create(EApi api) {
    EStringView filename;
#ifdef _WIN32
    filename = api==GL?"Engine/Renderer/GL/Engine_Renderer_EXT_GL.dll":"Engine/Renderer/GL/Engine_Renderer_EXT_VK.dll";
    dll = LoadLibraryA(filename);
#elif defined(__linux__)
    filename = api==GL?"Engine/Renderer/GL/libEngine_Renderer_EXT_GL.so":"Engine/Renderer/GL/libEngine_Renderer_EXT_VK.so";
    dll = dlopen(filename, RTLD_NOW);
#endif

    if (!dll) {
        EERROR("Failed to load renderer library from '%s'!", filename);
        return nullptr;
    }

#ifdef _WIN32
    PFN_GetRenderDeviceLayout getLayout = (PFN_GetRenderDeviceLayout)GetProcAddress(dll, "ERenderDevice_GetLayout");
#elif defined(__linux__)
    PFN_GetRenderDeviceLayout getLayout = (PFN_GetRenderDeviceLayout)dlsym(dll, "ERenderDevice_GetLayout");
#endif

    if (!getLayout) {
        EERROR("Failed to get render device layout function!");
#ifdef _WIN32
        FreeLibrary(dll);
#elif defined(__linux__)
        dlclose(dll);
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
    dlclose(dll);
#endif
}
