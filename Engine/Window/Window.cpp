#include "Window.hpp"
#include "Engine/Common/Assert.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

#include <cstdlib>
#include <SDL3/SDL.h>

Eclipse::Window::WindowState* Eclipse::Window::CreateWindow(WindowCreateInfo& create_info) {
    WindowState* ws = Eclipse::MemoryPool::Allocate<WindowState>(create_info.mempool, 1);
    ECL_ASSERT(ws != nullptr, "Window Could not be allocated!");

    SDL_Window* window;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
    if (create_info.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (create_info.resizable)  flags |= SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(create_info.title.c_str(), create_info.size.w, create_info.size.h, flags);

    ECL_ASSERT(window != nullptr, "Failed to create window!");

    ws->ptr = reinterpret_cast<void*>(window);

    return ws;
}

void Eclipse::Window::DestroyWindow(WindowState* state) {
    SDL_DestroyWindow((SDL_Window*)state->ptr);
    SDL_Quit();
}

bool Eclipse::Window::ShouldClose(WindowState* state) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return true;
        }
    }

    return false;
}

void Eclipse::Window::Update(WindowState* state) {

}
