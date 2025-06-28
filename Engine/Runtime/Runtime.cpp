#include <Engine/Window/Window.hpp>
#include <Engine/Common/Memory.hpp>
#include <Engine/Graphics/Server.hpp>
#include <Engine/Common/Time.hpp>
#include <print>

bool shouldClose = false;
Eclipse::IntVec2 window_size = {0, 0};
Eclipse::Time::TimeData timedata{};

void ProccessEvents(Eclipse::EventPool::EventPool* pool) {
    for (size_t i = 0; i < pool->enumerator; i++) {
        Eclipse::Event::Event& event = pool->events[i];
        switch (event.type) {
            case Eclipse::Event::Type::Exit:
                shouldClose = true;
                break;
            case Eclipse::Event::Type::WindowResized:
                Eclipse::IntVec2& size = std::get<Eclipse::Event::WindowResizeEvent>(event.data).size;
                window_size = size;
                break;
        }
    }

    pool->events.clear();
    pool->enumerator = 0;
}

int main(int argc, char* argv[]) {
    Eclipse::MemoryPool::CreateInfo mempool_create_info{};
    mempool_create_info.size = 4096;

    Eclipse::MemoryPool::MemoryPool* main_memory_pool = Eclipse::MemoryPool::Create(mempool_create_info);

    Eclipse::EventPool::CreateInfo eventpool_create_info{};
    eventpool_create_info.max_events = 32;
    eventpool_create_info.pool = main_memory_pool;
    eventpool_create_info.dispatch_file = "none.txt";

    Eclipse::EventPool::EventPool* event_pool = Eclipse::EventPool::Create(eventpool_create_info);

    Eclipse::Window::CreateInfo window_create_info{};
    window_create_info.title = "Hello, World!";
    window_create_info.size = {600, 400};
    window_create_info.resizable = true;
    window_create_info.fullscreen = false;
    window_create_info.adaptiveresolution = false;
    window_create_info.mempool = main_memory_pool;
    window_create_info.eventpool = event_pool;

    Eclipse::Window::Window* main_window = Eclipse::Window::Create(window_create_info);

    Eclipse::GraphicsServer::CreateInfo graphicsserver_create_info{};
    graphicsserver_create_info.mempool = main_memory_pool;
    graphicsserver_create_info.window = main_window;

    Eclipse::GraphicsServer::GraphicsServer* graphics_server = Eclipse::GraphicsServer::Create(graphicsserver_create_info);

    Eclipse::FloatVec4 clearcolor = {0.0f, 0.0f, 0.0f, 0.0f};

    while (!shouldClose) {
        Eclipse::Window::Poll(main_window);
        ProccessEvents(event_pool);

        graphics_server->clear(clearcolor);

        Eclipse::Window::Update(main_window);
        Eclipse::Time::Update();
        timedata = Eclipse::Time::Get();
    }

    Eclipse::Window::Destroy(main_window);
    Eclipse::MemoryPool::Destroy(main_memory_pool);
}
