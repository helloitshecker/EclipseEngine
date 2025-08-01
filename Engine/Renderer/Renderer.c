#include "Renderer.h"

#ifdef _WIN32
#include <windows.h>
#endif

ERenderDevice_Layout* eRenderer_Create(ERenderer_CreateInfo* createInfo) {
        HMODULE dll = LoadLibraryA(createInfo->fileName);
        ERUN_MSG(!dll, "Failed to load renderer library from '%s'!", createInfo->fileName);
        if (!dll) return nullptr;

        PFN_GetRenderDeviceLayout getLayout = (PFN_GetRenderDeviceLayout)GetProcAddress(dll, "ERenderDevice_GetLayout");
        ERUN_MSG(!getLayout, "Failed to get render device layout function!");
        if (!getLayout) {
                FreeLibrary(dll);
                return nullptr;
        }

        return getLayout();
}

void eRenderer_Destroy(ERenderDevice_Layout* renderDevice) {

}