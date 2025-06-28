#include "EventPool.hpp"
#include <Engine/Common/Assert.hpp>

Eclipse::EventPool::EventPool* Eclipse::EventPool::Create(CreateInfo& create_info) {
    EventPool* pool = Eclipse::MemoryPool::Allocate<EventPool>(create_info.pool, 1);
    ECL_ASSERT(pool != nullptr, "Failed to allocate memory!");

    pool->events = std::vector<Eclipse::Event::Event>();
    pool->events.reserve(create_info.max_events);
    pool->dispatch_file = create_info.dispatch_file;

    return pool;
}
