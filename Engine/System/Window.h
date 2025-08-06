#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/Core/Events.h>

#ifdef _WIN32
  #ifdef ENGINE_SYSTEM_EXPORTS
    #define ENGINE_API __declspec(dllexport)
  #else
    #define ENGINE_API __declspec(dllimport)
  #endif
#else
  #define ENGINE_API
#endif


typedef struct {
        void* ptr;              ///<SDL_Window Struct Pointer
        void* glcontext;        ///<OpenGL Context. Value is NULL or nullptr when not using OpenGL as Renderer backend
} EWindow;

typedef struct {
        char* title;            ///<Title of Window
        i32 width;              ///<Width of Window in Pixels (Not for retina displays)
        i32 height;             ///<Height of Window in Pixels (Not for retina displays)
        bool resizable;         ///<Can Window Resize
        bool fullscreen;        ///<Open as Fullscreen Window
        EApi render_api;        ///<Rendering Backend (Required for Initializing Window)
        EMemory* memory;        ///<Allocator for Allocating Memory for Window
        EEventQueue* events;    ///<Event Queue to submit events of Window
} EWindow_CreateInfo;

/**
 * @brief Creates a Window
 * @param info CreateInfo for window
 * @return EWindow* handle
 */
EWindow* eWindow_Create(EWindow_CreateInfo* info);

/**
 * @brief Closes Window
 * @param window Window Handle
 */
void eWindow_Destroy(EWindow* window);

/**
 * @brief Gets all events of window and adds them to event queue
 * @param window Window Handle
 * @param events Event Queue
 */
void eWindow_PollEvent(EWindow* window, EEventQueue* events);

/**
 * @brief Presents Image to a window by Swapping Buffers
 * @param window Window Handle
 */
void eWindow_Swap(EWindow* window);

ENGINE_API void eWindow_Simple_SetMemoryAndEventQueue(EMemory* memory, EEventQueue* events);
ENGINE_API EWindow* eWindow_Simple_Create(EStringView title, i32 width, i32 height, bool resizable, bool fullscreen, EApi api);
ENGINE_API void eWindow_Simple_PollEvent();
ENGINE_API void eWindow_Simple_Swap();
ENGINE_API EWindow* eWindow_Simple_GetWindowHandle();
ENGINE_API Vec2 eWindow_Simple_GetWindowSize();
ENGINE_API void eWindow_Simple_Destroy();