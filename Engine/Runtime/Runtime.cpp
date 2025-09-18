#include <Engine/Core/Window.hpp>
#include <Engine/Core/EventManager.hpp>
#include <Engine/Core/Error.hpp>
#include <Engine/Graphics/RenderDevice.hpp>
#include <Engine/Core/JobSystem.hpp>
#include <Engine/Core/VirtualFS.hpp>

bool quit = false;
#ifndef NDEBUG
constexpr bool debug = true;
#else
constexpr bool debug = false;-
#endif

static void HandleEvents(Eclipse::EventManager& event_manager) {
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

    const auto resources_folder = Eclipse::FileSystem::SearchUpTreeRecursiveN("Resources", 5);
    if (!resources_folder) {
        Eclipse::LogError("Failed to load resources file :(");
        std::exit(EXIT_FAILURE);
    }

    const Eclipse::VirtualFS::CreateInfo vfs_create_info {
        .mount_point = "res://",
        .folder = resources_folder.value(),
    };

    Eclipse::Error vfs_ec;
    Eclipse::VirtualFS vfs (vfs_create_info, vfs_ec);

    if (vfs_ec) {
        Eclipse::LogError("[VIRTUAL FILESYSTEM] {}", vfs_ec.message);
        std::exit(EXIT_FAILURE);
    }

      Eclipse::EventManager event_manager;

      Eclipse::Window::CreateInfo win_info {
            .title = "Eclipse Runtime",
            .width = 800,
            .height = 600,
            .resizable = true,
            .flags = Eclipse::Window::Flags::WINDOWED
      };

      Eclipse::Error window_ec;
      Eclipse::Window win(win_info, window_ec);

      if (window_ec) {
          Eclipse::LogError("[WINDOW] {}", window_ec.message);
          std::exit(EXIT_FAILURE);
      }

      Eclipse::RenderDevice::CreateInfo render_dev_info {
            .extensions = win.GetVulkanCreationData(),
            .window = win,
            .vfs = vfs,
            .debug = debug
      };

      Eclipse::RenderDevice render_device(render_dev_info);

      while (!quit) {
            win.SubmitEvents(event_manager);
            HandleEvents(event_manager);

            render_device.DrawFrame();
      }
}
