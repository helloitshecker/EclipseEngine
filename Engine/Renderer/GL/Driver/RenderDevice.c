#include "RenderDevice.h"
#include <SDL3/SDL.h>

thread_local EWindow* gWindow;

bool EXT_GL_CreateDevice(EWindow* window) {
        EDEBUG("Hello from Render Device!");

        SDL_GLContext context = SDL_GL_CreateContext(window->ptr);
        SDL_GL_MakeCurrent(window->ptr, context);

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
                EERROR("Failed to initialize OpenGL context");
                return EFAILURE;
        } else {
                EDEBUG("Successfully initialized OpenGL context");
        }

        gWindow = window;

        return ESUCCESS;
}

void EXT_GL_DestroyDevice() {
        EDEBUG("Goodbye from Render Device!");
}

void EXT_GL_Clear(u8 r, u8 g, u8 b, u8 a) {
        glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EXT_GL_Set_Viewport(i32 x, i32 y, i32 width, i32 height) {
        glViewport(x, y, width, height);
}

void EXT_GL_Set_Vsync(bool on) {
        if (SDL_GL_SetSwapInterval(on) < 0) {
                EERROR("Failed to %s vsync!", on?"disable":"enable");
        }
}

static ERenderDevice_Layout layout = {
        .CreateDevice = EXT_GL_CreateDevice,
        .DestroyDevice = EXT_GL_DestroyDevice,
        .Clear = EXT_GL_Clear,
        .SetViewport = EXT_GL_Set_Viewport,
        .SetVsync = EXT_GL_Set_Vsync,
};

EEXPORT ERenderDevice_Layout* ERenderDevice_GetLayout(void) {
        return &layout;
}