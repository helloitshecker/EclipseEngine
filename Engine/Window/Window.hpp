#pragma once

#include <Engine/Common/Memory.hpp>
#include <Engine/Common/Types.hpp>
#include <Engine/Common/Assert.hpp>
#include <string>

namespace Eclipse {
    namespace Window {
        struct WindowCreateInfo {
            std::string title;
            Eclipse::IntVec2 size;
            bool resizable;
            bool fullscreen;
            bool adaptiveresolution;
            Eclipse::MemoryPool::MemoryPool* mempool;
        };
        struct WindowState {
            void* ptr;
            Eclipse::IntVec2 size;
        };

        WindowState* CreateWindow(WindowCreateInfo&);
        void DestroyWindow(WindowState*);

        bool ShouldClose(WindowState*);
        void Update(WindowState*);
    }
}
