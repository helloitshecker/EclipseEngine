#pragma once

#include <Engine/Window/Window.hpp>
#include <Engine/Common/Memory.hpp>

namespace Eclipse {
    namespace GraphicsServer {

        struct CreateInfo {
            Eclipse::Window::Window* window;
            Eclipse::Memory::Pool::Pool* mempool;
        };

        struct GraphicsServer {
            void (*clear)(Eclipse::FloatVec4&);
        };

        GraphicsServer* Create(CreateInfo&);
        void Destroy(GraphicsServer*);
    }
}
