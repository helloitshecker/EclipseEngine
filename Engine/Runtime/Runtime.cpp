#include <Engine/Core/Window.hpp>
#include <Engine/Core/EventManager.hpp>
#include <Engine/Core/Error.hpp>

bool quit = false;

void HandleEvents(Eclipse::EventManager& event_manager) {
        while (true) {
                auto event_opt = event_manager.Pop();
                if (!event_opt.has_value())
                        break;

                const auto& ev = event_opt.value();
                switch (ev.type) {
                        case Eclipse::EventManager::EventType::KEY_PRESSED:
                                EINFO("Key Pressed: {}", ev.data.key_code);
                                break;

                        case Eclipse::EventManager::EventType::KEY_RELEASED:
                                EINFO("Key Released: {}", ev.data.key_code);
                                break;

                        case Eclipse::EventManager::EventType::MOUSE_PRESSED:
                                EINFO("Mouse Button Pressed: {}", ev.data.key_code);
                                break;

                        case Eclipse::EventManager::EventType::MOUSE_RELEASE:
                                EINFO("Mouse Button Released: {}", ev.data.key_code);
                                break;

                        case Eclipse::EventManager::EventType::MOUSE_MOVE:
                                EINFO("Mouse Moved to ({}, {}), Rel: ({}, {})",
                                      ev.data.mouse_position.pos[0],
                                      ev.data.mouse_position.pos[1],
                                      ev.data.mouse_position.rel_pos[0],
                                      ev.data.mouse_position.rel_pos[1]);
                                break;

                        case Eclipse::EventManager::EventType::MOUSE_WHEEL:
                                EINFO("Mouse Wheel: x={}, y={}",
                                      ev.data.scroll[0], ev.data.scroll[1]);
                                break;

                        case Eclipse::EventManager::EventType::WINDOW_RESIZED:
                                EINFO("Window resized to {}x{}",
                                      GetUpper32(ev.data.window_size),
                                      GetLower32(ev.data.window_size));
                                break;

                        case Eclipse::EventManager::EventType::WINDOW_CLOSE:
                                EINFO("Window closing");
                                quit = true;
                                break;

                        default:
                                EINFO("Unhandled event type: {}", static_cast<int>(ev.type));
                                break;
                }
        }
}


int main(int argc, char* argv[]) {
        Eclipse::EventManager event_manager;

        const Eclipse::Window::CreateInfo windowCreateInfo{};
        Eclipse::Window window {windowCreateInfo};

        while (!quit) {
                window.SubmitEvents(event_manager);
                HandleEvents(event_manager);
        }
}