#include "Window.hpp"
#include "Engine/Common/Assert.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

//#include <cstdlib>
#include <SDL3/SDL.h>

Eclipse::Window::Window* Eclipse::Window::Create(CreateInfo& create_info) {
    Window* ws = Eclipse::MemoryPool::Allocate<Window>(create_info.mempool, 1);
    ECL_ASSERT(ws != nullptr, "Window Could not be allocated!");

    SDL_Window* window;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
    if (create_info.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (create_info.resizable)  flags |= SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(create_info.title.c_str(), create_info.size.w, create_info.size.h, flags);

    ECL_ASSERT(window != nullptr, "Failed to create window!");

    ws->ptr = reinterpret_cast<void*>(window);
    ws->eventpool = create_info.eventpool;

    return ws;
}

void Eclipse::Window::Destroy(Window* state) {
    SDL_DestroyWindow((SDL_Window*)state->ptr);
    SDL_Quit();
}

void Eclipse::Window::Poll(Window* state) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        Eclipse::Event::Event current{};

        bool valid = true;

        switch (event.type) {
            case SDL_EVENT_QUIT:
                current.type = Eclipse::Event::Type::Exit;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                current.type = Eclipse::Event::Type::WindowResized;
                current.data = Eclipse::Event::WindowResizeEvent {
                    {
                        static_cast<int>(event.window.data1),
                        static_cast<int>(event.window.data2)
                    }
                };
                break;
            default:
                valid = false;
                break;
        }

        if (valid) {
            state->eventpool->events.push_back(current);
            state->eventpool->enumerator++;
        }
    }
}

void Eclipse::Window::Update(Window* state) {

}
