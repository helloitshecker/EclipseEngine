#include "EventManager.hpp"

namespace Eclipse {
        EventManager::EventManager() : event_queue(256), pointer_end(0) {
        }

        void EventManager::Push(const Event &event) {
                event_queue[pointer_end] = event;
                pointer_end++;
        }

        std::optional<EventManager::Event> EventManager::Pop() {
                if (pointer_end == 0) return std::nullopt;
                pointer_end--;
                return event_queue[pointer_end];
        }

}