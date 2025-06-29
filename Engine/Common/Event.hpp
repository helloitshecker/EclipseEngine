#pragma once

#include <SDL3/SDL.h>
#include <Engine/Common/Types.hpp>

namespace Eclipse {
    namespace Event {

        enum class Type {
            KeyDown,
            KeyUp,
            MouseDown,
            MouseUp,
            MouseMove,
            WindowResize,
            Exit
        };

        typedef struct KeyDown {
            uint32_t code;
        } KeyDown;

        typedef struct KeyUp {
            uint32_t code;
        } KeyUp;

        typedef struct MouseDown {
            uint32_t code;
        } MouseDown;

        typedef struct MouseUp {
            uint32_t code;
        } MouseUp;

        typedef struct MouseMove {
            Eclipse::FloatVec2 position;
            Eclipse::FloatVec2 relative_position;
        } MouseMove;

        typedef struct WindowResize {
            Eclipse::IntVec2 size;
        } WindowResize;

        union Data {
            KeyDown keyDown;
            KeyUp keyUp;
            MouseDown mouseDown;
            MouseUp mouseUp;
            MouseMove mouseMove;
            WindowResize windowResize;
        };

        struct Event {
            Type type;
            Data data;
        };
    }
}
