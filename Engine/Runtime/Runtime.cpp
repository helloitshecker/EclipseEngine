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

    const auto resource_file_path = Eclipse::FileSystem::SearchUpTreeRecursiveFileN("Resources/Resources.epak", 5);
    if (resource_file_path == std::nullopt) {
        Eclipse::LogError("Failed to find Resource File!");
        abort();
    }

    Eclipse::LogInfo("Resource file found at: \"{}\"", resource_file_path.value().string());

    // Eclipse::VirtualFS::GenerateResourceFile();
    // abort();

    // const auto fp = Eclipse::FileSystem::SearchUpTreeRecursiveFileN("Engine/Shaders/Source/triangle.frag", 5);
    // const auto bincont = Eclipse::FileSystem::ReadBinaryFile(fp.value());
    // const auto wp = "../../Resources/triangle.frag.ebin";

    // Eclipse::FileSystem::WriteBinaryFile(wp, bincont.value());
    // abort();

    Eclipse::VirtualFS vfs {resource_file_path.value(), "res://"};

    // const auto vert_code_path = Eclipse::FileSystem::SearchUpTreeRecursiveFileN("Engine/Shaders/Source/triangle.vert", 5);
    // const auto vert_code = Eclipse::FileSystem::ReadBinaryFile(vert_code_path.value());
    // const auto frag_code_path = Eclipse::FileSystem::SearchUpTreeRecursiveFileN("Engine/Shaders/Source/triangle.frag", 5);
    // const auto frag_code = Eclipse::FileSystem::ReadBinaryFile(frag_code_path.value());

    // vfs.create_file("res://triangle.vert", vert_code.value());
    // vfs.create_file("res://triangle.frag", frag_code.value());

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
            .vfs = vfs,
            .debug = debug
      };

      Eclipse::RenderDevice render_device{render_dev_info};

      while (!quit) {
            win.SubmitEvents(event_manager);
            HandleEvents(event_manager);

            render_device.DrawFrame();
      }
}
