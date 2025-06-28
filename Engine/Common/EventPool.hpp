#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <Engine/Common/Event.hpp>
#include <Engine/Common/Memory.hpp>

namespace Eclipse {
    namespace EventPool {
        struct CreateInfo {
            std::size_t max_events;
            std::string dispatch_file;
            Eclipse::MemoryPool::MemoryPool* pool;
        };
        struct EventPool {
            std::vector<Eclipse::Event::Event> events;
            size_t enumerator;
            std::string dispatch_file;
        };

        EventPool* Create(CreateInfo&);
    }
}
