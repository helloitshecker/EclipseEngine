#include <Engine/Core/Debug.h>
#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>

#include "Engine/System/Window.h"

void ProcessPollEvents(bool* running, EEvents* events) {
        for (u8 i = 0; i < events->count; i++) {
                switch (events->types[i]) {
                        case EEVENT_TYPE_WINDOW_RESIZE: {
                                EINFO("Window Resized to (%d, %d)", events->events[i].window_resize.width, events->events[i].window_resize.height);
                        } break;
                        case EEVENT_TYPE_WINDOW_CLOSE: {
                                EINFO("Window Close Requested!");
                                *running = false;
                        } break;
                        default: ;
                }
        }
}

int main() {
        // Initialize Memory Pool
        EMemory* memory = eMemory_Create(8096);

        // Initialize Events System
        EEvents_CreateInfo events_CreateInfo = {memory};
        EEvents* events = eEvents_Create(&events_CreateInfo);

        // Initialize Window
        EWindow_CreateInfo windowCreateInfo = {0};
        windowCreateInfo.title = "Eclipse Engine Runtime!";
        windowCreateInfo.width = 1024;
        windowCreateInfo.height = 768;
        windowCreateInfo.resizable = true;
        windowCreateInfo.fullscreen = false;
        windowCreateInfo.render_api = GL;
        windowCreateInfo.memory = memory;
        windowCreateInfo.events = events;
        EWindow* window = eWindow_Create(&windowCreateInfo);

        bool running = true;
        while (running) {
                eWindow_PollEvent(window, events);
                ProcessPollEvents(&running, events);


        }

        eWindow_Destroy(window);
        eMemory_Destroy(memory);
}
