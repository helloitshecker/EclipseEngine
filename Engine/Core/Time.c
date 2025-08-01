#include "Time.h"

#include <SDL3/SDL.h>

f32 eTime_GetTicks() {
        return SDL_GetTicks();
}

f32 eTime_GetTicksPerSecond() {
        return SDL_GetTicks()/1000;
}


f64 eTime_GetHighResClock() {
        return (f64) SDL_GetPerformanceCounter()/SDL_GetPerformanceFrequency();
}