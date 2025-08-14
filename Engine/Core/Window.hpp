#pragma once

#include <Engine/Core/Types.hpp>
#include <Engine/Core/EventManager.hpp>
#include <string>

namespace Eclipse {
        class Window {
        public:
                enum class Flags {
                        EXCLUSIVE_FULLSCREEN,
                        BORDERLESS_FULLSCREEN,
                        WINDOWED,
                        BORDERLESS_WINDOWED,
                };

                struct CreateInfo {
                        std::string title               = "Eclipse Engine";
                        i32 width                       = -1;
                        i32 height                      = -1;
                        b32 resizable                   = false;
                        Flags flags                     = Flags::BORDERLESS_FULLSCREEN;
                };

                explicit Window(const CreateInfo&);
                ~Window();

                void SubmitEvents(EventManager&);
        private:
                static u64 get_monitor_pixels();
                void* handle;
        };
}
