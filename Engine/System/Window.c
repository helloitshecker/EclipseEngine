#include "Window.h"

#include <SDL3/SDL.h>
#include <Engine/Core/Types.h>
#include <Engine/Core/Debug.h>


static thread_local EMemory* g_memory = nullptr;
static thread_local EEventQueue* g_eventqueue = nullptr;
static thread_local EWindow* g_window = nullptr;

EWindow* eWindow_Create(EWindow_CreateInfo* info) {
        SDL_Init(SDL_INIT_VIDEO);
        EWindow* pWindow = eMemory_Alloc(info->memory, sizeof(EWindow));
        if (!pWindow) {
                EERROR("Failed to allocate window memory");
                return nullptr;
        }

        SDL_WindowFlags window_flags = (info->resizable?SDL_WINDOW_RESIZABLE:0) | (info->fullscreen?SDL_WINDOW_FULLSCREEN:0);
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

        g_window = returnPtr;
        g_memory = info->memory;
        g_eventqueue = info->events;

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

void eWindow_Simple_SetMemoryAndEventQueue(EMemory* memory, EEventQueue* events) {
        EDEBUG("Using Window Simple Mode; Setting Memory and Queue!");
        g_memory = memory;
        g_eventqueue = events;
}

EWindow* eWindow_Simple_Create(EStringView title, i32 width, i32 height, bool resizable, bool fullscreen, EApi api) {
        EWindow_CreateInfo s_window_create_info = {
                .title =  title,
                .width = width,
                .height = height,
                .resizable = resizable,
                .fullscreen = fullscreen,
                .render_api = api,
                .memory = g_memory,
                .events = g_eventqueue
        };

        g_window = eWindow_Create(&s_window_create_info);

        return g_window;
}


void eWindow_Simple_PollEvent() {
        eWindow_PollEvent(g_window, g_eventqueue);
}

void eWindow_Simple_Swap() {
        eWindow_Swap(g_window);
}

EWindow* eWindow_Simple_GetWindowHandle() {
        return g_window;
}

Vec2 eWindow_Simple_GetWindowSize() {
        Vec2 size = {0 ,0};
        SDL_GetWindowSize(g_window->ptr, &size.w, &size.h);
        return size;
}

void eWindow_Simple_Destroy() {
        eWindow_Destroy(g_window);
}