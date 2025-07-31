#include "Window.h"

#include <SDL3/SDL.h>
#include <Engine/Core/Types.h>
#include <Engine/Core/Debug.h>

EWindow* eWindow_Create(EWindow_CreateInfo* info) {
        EASSERT(SDL_Init(SDL_INIT_VIDEO) > 0);
        EWindow* pWindow = eMemory_Alloc(info->memory, sizeof(EWindow));
        EASSERT(pWindow);

        SDL_WindowFlags window_flags = info->resizable?SDL_WINDOW_RESIZABLE:0 | info->fullscreen?SDL_WINDOW_FULLSCREEN:0;
        if (info->render_api == GL) {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

                window_flags |= SDL_WINDOW_OPENGL;
        } else if (info->render_api == VK) {
                window_flags |= SDL_WINDOW_VULKAN;
        } else if (info->render_api ==  METAL) {
                window_flags |= SDL_WINDOW_METAL;
        }

        SDL_Window* sWindow = SDL_CreateWindow(info->title, info->width, info->height, window_flags);
        EASSERT_MSG(sWindow, "Error creating window!\nERROR: %s", SDL_GetError());

        EWindow* returnPtr = eMemory_Alloc(info->memory, sizeof(EWindow));
        returnPtr->ptr = (void*)sWindow;

        return returnPtr;
}

void eWindow_Destroy(EWindow* window) {
        SDL_DestroyWindow((SDL_Window*)window->ptr);
}

void eWindow_PollEvent(EWindow* window, EEvents* events) {
        SDL_Event s_event;
        events->count = 0;
        while (SDL_PollEvent(&s_event)) {
                switch (s_event.type) {
                        case SDL_EVENT_WINDOW_RESIZED: {
                                events->events[events->count].window_resize = (EEvent_Type_Window_Resize){
                                        s_event.window.data1,
                                        s_event.window.data2,
                                };
                                events->types[events->count] = EEVENT_TYPE_WINDOW_RESIZE;
                                events->count++;
                        } break;
                        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                        case SDL_EVENT_QUIT: {
                                events->types[events->count] = EEVENT_TYPE_WINDOW_CLOSE;
                                events->count++;
                        } break;
                }
        }
}