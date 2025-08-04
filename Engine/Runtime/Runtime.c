#include <Engine/Core/Debug.h>
#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/System/Window.h>
#include <Engine/Renderer/Renderer.h>
#include <Engine/Core/Time.h>
#include <Engine/Core/FileSystem.h>

ERenderDevice_Layout* rd_layout;
f64 delta;
bool vsync;

void ProcessPollEvents(bool* running, EEventQueue* events) {
        for (u8 i = 0; i < events->count; i++) {
                switch (events->types[i]) {
                        case EEVENT_TYPE_WINDOW_RESIZE: {
                                EINFO("Window Resized to (%d, %d)", events->queue[i].window_resize.width, events->queue[i].window_resize.height);
                                rd_layout->SetViewport(0, 0, events->queue[i].window_resize.width, events->queue[i].window_resize.height);
                        } break;
                        case EEVENT_TYPE_WINDOW_CLOSE: {
                                EINFO("Window Close Requested!");
                                *running = false;
                        } break;
                        case EEVENT_TYPE_WINDOW_FOCUS: {
                                if (events->queue[i].window_focus.gained) {
                                        EINFO("Window in focus!");
                                } else {
                                        EINFO("Window out focus!");
                                }
                        } break;
                        case EEVENT_TYPE_WINDOW_MINIMIZE: {
                                EINFO("Window Minimized!");
                        } break;
                        case EEVENT_TYPE_WINDOW_RESTORE: {
                                EINFO("Window Restored!");
                        } break;
                        case EEVENT_TYPE_WINDOW_MAXIMIZE: {
                                EINFO("Window Maximized!");
                        } break;
                        case EEVENT_TYPE_WINDOW_DISPLAY_CHANGE: {
                                EINFO("Window Display Changed to %llu!", events->queue[i].window_display.display_id);
                        } break;
                        case EEVENT_TYPE_MOUSE_MOVE: {
                                // const EEvent_Type_Mouse_Move mouse_move = events->queue[i].mouse_move;
                                // EINFO("Mouse Moved to x:%f y:%f relx:%f rely:%f", mouse_move.x, mouse_move.y, mouse_move.rx, mouse_move.ry);
                        } break;
                        case EEVENT_TYPE_MOUSE_BUTTON: {
                                EINFO("Mouse Button Clicked! [%d][%s]", events->queue[i].mouse_button.code, events->queue[i].mouse_button.down?("PRESSED"):("RELEASED"));
                        } break;
                        case EEVENT_TYPE_KEYBOARD: {
                                const EEvent_Type_Keyboard keyboard = events->queue[i].keyboard;
                                EINFO("Keyboard Button [%d] %s!", keyboard.code, keyboard.down?("PRESSED"):("RELEASED"));
                                if (events->queue[i].keyboard.code == 118 && !events->queue[i].keyboard.down) {
                                        EINFO("Turning Vsync %s!", vsync?"OFF":"ON");
                                        rd_layout->SetVsync(vsync);
                                        vsync = !vsync;
                                }
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
        EEventQueue* events = eEvents_Create(&events_CreateInfo);

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

        ERenderer_CreateInfo rendererCreateInfo = {0};
#ifdef _WIN32
        rendererCreateInfo.fileName = "Engine/Renderer/GL/Engine_Renderer_EXT_GL.dll";
#elif __linux__
        rendererCreateInfo.fileName = "Engine/Renderer/GL/libEngine_Renderer_EXT_GL.so";
#endif
        rendererCreateInfo.api = GL;
        rd_layout = eRenderer_Create(&rendererCreateInfo);
        if (!rd_layout) {
                EERROR("Failed to create renderer!");
                return EFAILURE;
        }
        if (rd_layout->CreateDevice(window) == EFAILURE) {
                EERROR("Failed to initialize renderer!");
                return EFAILURE;
        }

        rd_layout->SetViewport(0, 0, windowCreateInfo.width, windowCreateInfo.height);

        bool running = true;
        f64 previous = 0;
        while (running) {
                const f64 current = eTime_GetSeconds();
                delta = current - previous;
                previous = current;

                // printf("\rFPS: %f         ",    1/delta);

                eWindow_PollEvent(window, events);
                ProcessPollEvents(&running, events);

                rd_layout->Clear(0,0,0,255);

                eWindow_Swap(window);
        }

        const EString* file1 = eFileSystem_ReadFile(memory, "../nigga.txt", false);
        EINFO("File content: %s", file1->content);

        rd_layout->DestroyDevice();
        eWindow_Destroy(window);
        eMemory_Destroy(memory);
}
