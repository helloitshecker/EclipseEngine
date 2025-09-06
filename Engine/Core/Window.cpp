#include "Window.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <Engine/Core/Error.hpp>


namespace Eclipse {

        u64 Window::get_monitor_pixels() {
                const SDL_DisplayMode* m = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
                if (!m) return 0;
                u64 ret = 0;
                // Sets upper 32 bits to width and lower 32 bits to height
                ret = SetUpper32(m->w) | SetLower32(m->h);
                return ret;
        }

        Window::Window(const CreateInfo & info): handle(nullptr) {
                // Init SDL
                if (!SDL_Init(SDL_INIT_VIDEO)) {
                        Eclipse::LogError("Failed to initialize SDL!");
                        return;
                }

                // If width or height is set to -1 then set window size to monitor size
                i32 width = info.width, height = info.height;
                if (info.width == -1 || info.height == -1) {
                        const u64 wh = get_monitor_pixels();
                        width = static_cast<i32>(GetUpper32(wh));
                        height = static_cast<i32>(GetLower32(wh));
                }

                // Setting Flags according to the window
                SDL_WindowFlags flags = SDL_WINDOW_VULKAN;
                switch (info.flags) {
                        case Flags::EXCLUSIVE_FULLSCREEN:       flags |= SDL_WINDOW_FULLSCREEN; break;
                        case Flags::BORDERLESS_FULLSCREEN:      flags |= SDL_WINDOW_BORDERLESS; break;
                        case Flags::WINDOWED:                                                   break;
                        case Flags::BORDERLESS_WINDOWED:        flags |= SDL_WINDOW_BORDERLESS; break;
                }
                if (info.resizable) flags |= SDL_WINDOW_RESIZABLE;

                // Creates a window here
                handle = SDL_CreateWindow(info.title.c_str(), width, height, flags);
                if (!handle) {
                        Eclipse::LogError("Failed to create window! Due to {}", SDL_GetError());
                        return;
                }

                // To make the window feel fullscreen
                if (info.flags == Flags::BORDERLESS_FULLSCREEN) SDL_SetWindowPosition(static_cast<SDL_Window*>(handle), 0, 0);
        }

        Window::~Window() {
                SDL_DestroyWindow(static_cast<SDL_Window *>(handle));
                SDL_Quit();
        }

        void Window::SubmitEvents(EventManager& event_manager) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                        switch (event.type) {
                                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                                case SDL_EVENT_WINDOW_DESTROYED: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_CLOSE);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_MINIMIZED: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_MINIMIZE);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_MAXIMIZED: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_MAXIMIZE);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_RESTORED: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_RESTORE);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_FULLSCREEN);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_RESIZED: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_RESIZED);
                                        ret.data.window_size = SetUpper32(event.window.data1) | SetUpper32(event.window.data2);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_MOVED: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_MOVED);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_FOCUS_GAINED: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_FOCUS_GAINED);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_WINDOW_FOCUS_LOST: {
                                        EventManager::Event ret(EventManager::EventType::WINDOW_FOCUS_LOST);
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_MOUSE_MOTION: {
                                        EventManager::Event ret(EventManager::EventType::MOUSE_MOVE);
                                        ret.data.mouse_position = {
                                                .pos = {event.motion.x , event.motion.y},
                                                .rel_pos = {event.motion.xrel, event.motion.yrel}
                                        };
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                                        EventManager::Event ret(EventManager::EventType::MOUSE_PRESSED);
                                        ret.data.key_code = event.button.button;
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_MOUSE_BUTTON_UP: {
                                        EventManager::Event ret(EventManager::EventType::MOUSE_RELEASE);
                                        ret.data.key_code = event.button.button;
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_MOUSE_WHEEL: {
                                        EventManager::Event ret(EventManager::EventType::MOUSE_WHEEL);
                                        ret.data.scroll[0] = event.wheel.x;
                                        ret.data.scroll[1] = event.wheel.y;
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_KEY_DOWN: {
                                        EventManager::Event ret(EventManager::EventType::KEY_PRESSED);
                                        ret.data.key_code = event.key.key;
                                        event_manager.Push(ret);
                                } break;
                                case SDL_EVENT_KEY_UP: {
                                        EventManager::Event ret(EventManager::EventType::KEY_RELEASED);
                                        ret.data.key_code = event.key.key;
                                        event_manager.Push(ret);
                                } break;
                        }
                }
        }

        const std::vector<const char*> Window::GetVulkanCreationData() {
                u32 count{};
                auto pExtensions = SDL_Vulkan_GetInstanceExtensions(&count);
                if (!pExtensions) {
                        Eclipse::LogError("Failed to get Vulkan extensions from SDL_Vulkan_GetInstanceExtensions");
                        return {};
                }

                SDL_Vulkan_GetInstanceExtensions(&count);
                const std::vector<const char*> extensions(pExtensions, pExtensions + count);

                return extensions;
        }


}
