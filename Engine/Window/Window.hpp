#pragma once

#include <Engine/Common/EventPool.hpp>
#include <Engine/Common/Memory.hpp>
#include <Engine/Common/Types.hpp>
#include <Engine/Common/Assert.hpp>
#include <Engine/Common/Event.hpp>
#include <string>

namespace Eclipse {
    namespace Window {
        struct CreateInfo {
            std::string title;
            Eclipse::IntVec2 size;
            bool resizable;
            bool fullscreen;
            bool adaptiveresolution;
            Eclipse::MemoryPool::MemoryPool* mempool;
            Eclipse::EventPool::EventPool* eventpool;
        };
        struct Window {
            void* ptr;
            Eclipse::EventPool::EventPool* eventpool;
        };

        Window* Create(CreateInfo&);
        void Destroy(Window*);

        void Poll(Window*);
        void Update(Window*);
    }
}
