#include <Engine/Window/Window.hpp>
#include <Engine/Common/Memory.hpp>

int main(int argc, char* argv[]) {
    Eclipse::MemoryPool::MemoryPoolCreateInfo mempool_create_info{};
    mempool_create_info.size = 4096;

    Eclipse::MemoryPool::MemoryPool* main_memory_pool = Eclipse::MemoryPool::CreateMemoryPool(mempool_create_info);

    Eclipse::Window::WindowCreateInfo window_create_info{};
    window_create_info.title = "Hello, World!";
    window_create_info.size = {600, 400};
    window_create_info.resizable = true;
    window_create_info.fullscreen = false;
    window_create_info.adaptiveresolution = false;
    window_create_info.mempool = main_memory_pool;

    Eclipse::Window::WindowState* main_window = Eclipse::Window::CreateWindow(window_create_info);

    while (!Eclipse::Window::ShouldClose(main_window)) {
        Eclipse::Window::Update(main_window);
    }

    Eclipse::Window::DestroyWindow(main_window);
    Eclipse::MemoryPool::DestroyMemoryPool(main_memory_pool);
}
