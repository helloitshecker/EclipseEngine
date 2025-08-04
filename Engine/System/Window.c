#include "Window.h"

#include <SDL3/SDL.h>
#include <Engine/Core/Types.h>
#include <Engine/Core/Debug.h>

EWindow* eWindow_Create(EWindow_CreateInfo* info) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                EERROR("Failed to initialize SDL");
                return nullptr;
        }
        EWindow* pWindow = eMemory_Alloc(info->memory, sizeof(EWindow));
        if (!pWindow) {
                EERROR("Failed to allocate window memory");
                return nullptr;
        }

        SDL_WindowFlags window_flags = info->resizable?SDL_WINDOW_RESIZABLE:0 | info->fullscreen?SDL_WINDOW_FULLSCREEN:0;
        if (info->render_api == GL) {
                EINFO("Initializing OpenGL Window!");
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

                window_flags |= SDL_WINDOW_OPENGL;
        } else if (info->render_api == VK) {
                EINFO("Initializing Vulkan Window!");
                window_flags |= SDL_WINDOW_VULKAN;
        } else if (info->render_api ==  METAL) {
                EINFO("Initializing Metal Window!");
                window_flags |= SDL_WINDOW_METAL;
        }

        SDL_Window* sWindow = SDL_CreateWindow(info->title, info->width, info->height, window_flags);
        if (!sWindow) {
                EERROR("Error creating window!\nERROR: %s", SDL_GetError());
                return nullptr;
        }

        EWindow* returnPtr = eMemory_Alloc(info->memory, sizeof(EWindow));
        returnPtr->ptr = (void*)sWindow;

        if (info->render_api == GL)
                returnPtr->glcontext = SDL_GL_CreateContext(sWindow);

        return returnPtr;
}

void eWindow_Destroy(EWindow* window) {
        SDL_DestroyWindow((SDL_Window*)window->ptr);
}

void eWindow_PollEvent(EWindow* window, EEventQueue* events) {
        SDL_Event s_event;
        events->count = 0;
        while (SDL_PollEvent(&s_event)) {
                switch (s_event.type) {
                        case SDL_EVENT_WINDOW_RESIZED: {
                                events->queue[events->count].window_resize = (EEvent_Type_Window_Resize){
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
                        case SDL_EVENT_WINDOW_FOCUS_GAINED: {
                                events->types[events->count] = EEVENT_TYPE_WINDOW_FOCUS;
                                events->queue[events->count].window_focus = (EEvent_Type_Window_Focus){true};
                                events->count++;
                        } break;
                        case SDL_EVENT_WINDOW_FOCUS_LOST: {
                                events->types[events->count] = EEVENT_TYPE_WINDOW_FOCUS;
                                events->queue[events->count].window_focus = (EEvent_Type_Window_Focus){false};
                                events->count++;
                        } break;
                        case SDL_EVENT_WINDOW_MINIMIZED: {
                                events->types[events->count] = EEVENT_TYPE_WINDOW_MINIMIZE;
                                events->count++;
                        } break;
                        case SDL_EVENT_WINDOW_RESTORED: {
                                events->types[events->count] = EEVENT_TYPE_WINDOW_RESTORE;
                                events->count++;
                        } break;
                        case SDL_EVENT_WINDOW_MAXIMIZED: {
                                events->types[events->count] = EEVENT_TYPE_WINDOW_MAXIMIZE;
                                events->count++;
                        } break;
                        case SDL_EVENT_WINDOW_DISPLAY_CHANGED: {
                                events->types[events->count] = EEVENT_TYPE_WINDOW_DISPLAY_CHANGE;
                                events->queue[events->count].window_display = (EEvent_Type_Window_Display_Change){
                                        s_event.display.displayID
                                };
                                events->count++;
                        } break;
                        case SDL_EVENT_MOUSE_MOTION: {
                                events->types[events->count] = EEVENT_TYPE_MOUSE_MOVE;
                                events->queue[events->count].mouse_move = (EEvent_Type_Mouse_Move){
                                        .x = s_event.motion.x,
                                        .y = s_event.motion.y,
                                        .rx = s_event.motion.xrel,
                                        .ry = s_event.motion.yrel
                                };
                                events->count++;
                        } break;
                        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                                events->types[events->count] = EEVENT_TYPE_MOUSE_BUTTON;
                                events->queue[events->count].mouse_button = (EEvent_Type_Mouse_Button){
                                        .down = true,
                                        .code = (u32)s_event.button.button
                                };
                                events->count++;
                        } break;
                        case SDL_EVENT_MOUSE_BUTTON_UP: {
                                events->types[events->count] = EEVENT_TYPE_MOUSE_BUTTON;
                                events->queue[events->count].mouse_button = (EEvent_Type_Mouse_Button){
                                        .down = false,
                                        .code = (u32)s_event.button.button
                                };
                                events->count++;
                        } break;
                        case SDL_EVENT_KEY_DOWN: {
                                events->types[events->count] = EEVENT_TYPE_KEYBOARD;
                                events->queue[events->count].keyboard = (EEvent_Type_Keyboard){
                                        .down = true,
                                        .code = (u32)s_event.key.key
                                };
                                events->count++;
                        } break;
                        case SDL_EVENT_KEY_UP: {
                                events->types[events->count] = EEVENT_TYPE_KEYBOARD;
                                events->queue[events->count].keyboard = (EEvent_Type_Keyboard){
                                        .down = false,
                                        .code = (u32)s_event.key.key
                                };
                                events->count++;
                        } break;
                        default: ;
                }
        }
}

void eWindow_Swap(EWindow* window) {
        SDL_GL_SwapWindow(window->ptr);
}