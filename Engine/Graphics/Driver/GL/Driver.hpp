#pragma once

#include <Engine/Window/Window.hpp>
#include <Engine/Common/Types.hpp>

namespace Eclipse {
    namespace GLServer {

        struct CreateInfo {
            Eclipse::Window::Window* window;
            Eclipse::Memory::Pool::Pool* mempool;
        };

        struct GLServer {
            void* glcontext;
        };

        GLServer* Create(CreateInfo&);
        void Destroy();

        void GL_Clear(Eclipse::FloatVec4&);
    }
}
