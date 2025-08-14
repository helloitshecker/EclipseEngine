#pragma once

#include <vector>
#include <optional>

#include <Engine/Core/Types.hpp>

namespace Eclipse {
        class EventManager {
        public:
                enum class EventType {
                        WINDOW_CLOSE,
                        WINDOW_MINIMIZE,
                        WINDOW_MAXIMIZE,
                        WINDOW_RESTORE,
                        WINDOW_FULLSCREEN,
                        WINDOW_RESIZED,
                        WINDOW_MOVED,
                        WINDOW_FOCUS_LOST,
                        WINDOW_FOCUS_GAINED,
                        MOUSE_MOVE,
                        MOUSE_PRESSED,
                        MOUSE_RELEASE,
                        MOUSE_WHEEL,
                        KEY_PRESSED,
                        KEY_RELEASED,
                };

                union Data {
                        u64 window_size;
                        struct mouse_position {float pos[2]; float rel_pos[2];} mouse_position;
                        float scroll[2];
                        u64 key_code;
                };

                struct Event {
                        EventType type;
                        Data data;

                        explicit Event(const EventType type) : type(type), data(0) {}
                        explicit Event() = default;
                };

                EventManager();
                std::optional<Event> Pop();
                void Push(const Event &);
        private:
                std::vector<Event> event_queue;
                // Event event_queue[256];
                u64 pointer_end;
        };
}