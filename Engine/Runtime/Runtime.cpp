#include <Engine/Core/Window.hpp>
#include <Engine/Core/EventManager.hpp>
#include <Engine/Core/Error.hpp>
#include <Engine/Graphics/RenderDevice.hpp>
#include <Engine/Core/JobSystem.hpp>

bool quit = false;
#ifndef NDEBUG
constexpr bool debug = true;
#else
constexpr bool debug = false;
#endif

void HandleEvents(Eclipse::EventManager& event_manager) {
      while (true) {
            auto event_opt = event_manager.Pop();
            if (!event_opt.has_value())
                  break;

            const auto& ev = event_opt.value();
            switch (ev.type) {
            case Eclipse::EventManager::EventType::KEY_PRESSED:
                  Eclipse::LogInfo("Key Pressed: {}", ev.data.key_code);
                  break;

            case Eclipse::EventManager::EventType::KEY_RELEASED:
                  Eclipse::LogInfo("Key Released: {}", ev.data.key_code);
                  break;

            case Eclipse::EventManager::EventType::MOUSE_PRESSED:
                  Eclipse::LogInfo("Mouse Button Pressed: {}", ev.data.key_code);
                  break;

            case Eclipse::EventManager::EventType::MOUSE_RELEASE:
                  Eclipse::LogInfo("Mouse Button Released: {}", ev.data.key_code);
                  break;

            case Eclipse::EventManager::EventType::MOUSE_MOVE:
                  // Eclipse::LogInfo("Mouse Moved to ({}, {}), Rel: ({}, {})",
                  //       ev.data.mouse_position.pos[0],
                  //       ev.data.mouse_position.pos[1],
                  //       ev.data.mouse_position.rel_pos[0],
                  //       ev.data.mouse_position.rel_pos[1]);
                  break;

            case Eclipse::EventManager::EventType::MOUSE_WHEEL:
                  Eclipse::LogInfo("Mouse Wheel: x={}, y={}",
                        ev.data.scroll[0], ev.data.scroll[1]);
                  break;

            case Eclipse::EventManager::EventType::WINDOW_RESIZED:
                  Eclipse::LogInfo("Window resized to {}x{}",
                        GetUpper32(ev.data.window_size),
                        GetLower32(ev.data.window_size));
                  break;

            case Eclipse::EventManager::EventType::WINDOW_CLOSE:
                  Eclipse::LogInfo("Window closing");
                  quit = true;
                  break;

            default:
                  Eclipse::LogInfo("Unhandled event type: {}", static_cast<int>(ev.type));
                  break;
            }
      }
}

int main(int argc, char* argv[]) {

      Eclipse::EventManager event_manager;

      Eclipse::Window::CreateInfo win_info {
            .title = "Window",
            .width = 800,
            .height = 600,
            .resizable = true,
            .flags = Eclipse::Window::Flags::WINDOWED
      };

      Eclipse::Window win{win_info};

      Eclipse::RenderDevice::CreateInfo render_dev_info {
            .extensions = win.GetVulkanCreationData(),
            .window = win,
            .debug = debug
      };

      Eclipse::RenderDevice render_device{render_dev_info};

      while (!quit) {
            win.SubmitEvents(event_manager);
            HandleEvents(event_manager);

            render_device.DrawFrame();
      }
}