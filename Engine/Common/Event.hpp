#pragma once

#include <SDL3/SDL.h>
#include <Engine/Common/Types.hpp>
#include <variant>

namespace Eclipse {
    namespace Event {

        enum class Type {
          None = 0,
          KeyPressed,
          KeyReleased,
          MouseMoved,
          WindowResized,
          Exit
        };

        struct KeyEvent {
            uint32_t key_code;
        };

        struct MouseMoveEvent {
            Eclipse::IntVec2 pos;
        };

        struct WindowResizeEvent {
            Eclipse::IntVec2 size;
        };

        using EventData = std::variant<
            std::monostate,
            KeyEvent,
            MouseMoveEvent,
            WindowResizeEvent
        >;

        struct Event {
            Type type;
            EventData data;
        };
    }
}
