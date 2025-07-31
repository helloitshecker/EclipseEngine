#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Memory.h>
#include <Engine/Core/Debug.h>

typedef struct  {
       u32 width;
       u32 height;
} EEvent_Type_Window_Resize;

typedef union {
       EEvent_Type_Window_Resize window_resize;
} EEvent;

typedef enum {
       EEVENT_TYPE_WINDOW_RESIZE,
       EEVENT_TYPE_WINDOW_CLOSE,
} EEvent_Type;

typedef struct {
       EEvent events[256];
       EEvent_Type types[256];
       u8 count;
} EEvents;

typedef struct {
       EMemory* memory;
} EEvents_CreateInfo;

inline EEvents* eEvents_Create(EEvents_CreateInfo* createInfo) {
       return eMemory_Alloc(createInfo->memory, sizeof(EEvents));
}