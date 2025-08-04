#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/Core/Debug.h>

typedef struct  {
       u32 width;
       u32 height;
} EEvent_Type_Window_Resize;

typedef struct {
       bool gained;
} EEvent_Type_Window_Focus;

typedef struct {
       u64 display_id;
} EEvent_Type_Window_Display_Change;

typedef struct {
       f32 x;
       f32 y;
       f32 rx;
       f32 ry;
} EEvent_Type_Mouse_Move;

typedef struct {
       b32 down;
       u32 code;
} EEvent_Type_Mouse_Button;

typedef struct {
       b32 down;
       u32 code;
} EEvent_Type_Keyboard;

/**
 * @brief Union to store all types of event's data
 * This makes it a common interface for all events
 * This contains all the data related to the specific event
 */
typedef union {
       EEvent_Type_Window_Resize window_resize;
       EEvent_Type_Window_Focus  window_focus;
       EEvent_Type_Window_Display_Change window_display;
       EEvent_Type_Mouse_Move mouse_move;
       EEvent_Type_Mouse_Button mouse_button;
       EEvent_Type_Keyboard keyboard;
} EEvent;


/**
 * @brief Type of event
 */
typedef enum {
       EEVENT_TYPE_WINDOW_RESIZE,
       EEVENT_TYPE_WINDOW_CLOSE,
       EEVENT_TYPE_WINDOW_FOCUS,
       EEVENT_TYPE_WINDOW_MINIMIZE,
       EEVENT_TYPE_WINDOW_RESTORE,
       EEVENT_TYPE_WINDOW_MAXIMIZE,
       EEVENT_TYPE_WINDOW_DISPLAY_CHANGE,
       EEVENT_TYPE_MOUSE_MOVE,
       EEVENT_TYPE_MOUSE_BUTTON,
       EEVENT_TYPE_KEYBOARD,

} EEvent_Type;

/**
 * @brief Queue to store all events
 */
typedef struct {
       EEvent queue[256];          ///<All Queue Objects
       EEvent_Type types[256];     ///<All Queue Objects Types
       u8 count;                   ///<Total Queues with data which should be used
} EEventQueue;

/**
 * @brief CreateInfo struct which takes in EMemory* for memory allocation
 */
typedef struct {
       EMemory* memory;            ///<Memory Struct for use with eMemory_Alloc
} EEvents_CreateInfo;

/**
 *
 * @param createInfo Allocates Memory for Event Queue
 * @return Event Queue struct Pointer
 */
static inline EEventQueue* eEvents_Create(EEvents_CreateInfo* createInfo) {
       return eMemory_Alloc(createInfo->memory, sizeof(EEventQueue));
}