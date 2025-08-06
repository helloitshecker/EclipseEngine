#include <Engine/Core/Debug.h>
#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/System/Window.h>
#include <Engine/Renderer/Renderer.h>
#include <Engine/Core/Time.h>
#include <Engine/Core/FileSystem.h>
#include <Engine/Scripting/Scripting.h>

ERenderDevice_Layout* rd_layout;
f64 delta;
bool vsync;
i32 g_width;
i32 g_height;

void ProcessPollEvents(bool* running, EEventQueue* events) {
        for (u8 i = 0; i < events->count; i++) {
                switch (events->types[i]) {
                        case EEVENT_TYPE_WINDOW_RESIZE: {
                                EINFO("Window Resized to (%d, %d)", events->queue[i].window_resize.width, events->queue[i].window_resize.height);
                                // rd_layout->SetViewport(0, 0, events->queue[i].window_resize.width, events->queue[i].window_resize.height);
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
                                        // rd_layout->SetVsync(vsync);
                                        vsync = !vsync;
                                }
                        } break;
                        default: ;
                }
        }
}

int main() {
        // Initialize Memory Pool
        EMemory* memory = eMemory_Create(18096);

        // Initialize Scripting
        eScript_Create();

        // Initialize Events System
        EEvents_CreateInfo events_CreateInfo = {memory};
        EEventQueue* events = eEvents_Create(&events_CreateInfo);

        eWindow_Simple_SetMemoryAndEventQueue(memory, events);
        eScript_Run();

        eScript_CallInit();

        EWindow* window = eWindow_Simple_GetWindowHandle();
        Vec2 w_size = eWindow_Simple_GetWindowSize();
        g_width = w_size.w;
        g_height = w_size.h;

        bool running = true;
        f64 previous = 0;
        while (running) {
                const f64 current = eTime_GetSeconds();
                delta = current - previous;
                previous = current;

                eScript_Update(delta);

                // printf("\rFPS: %f         ",    1/delta);

                eWindow_Simple_PollEvent();
                ProcessPollEvents(&running, events);

                // rd_layout->Clear(0,0,0,255);

                eWindow_Simple_Swap();
        }

        const EString* file1 = eFileSystem_ReadFile(memory, "../nigga.txt", false);
        EINFO("File content: %s", file1->content);

        // rd_layout->DestroyDevice();
        eWindow_Simple_Destroy(window);
        eScript_Destroy();
        eMemory_Destroy(memory);
}
