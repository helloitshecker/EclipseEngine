#include "Time.h"

#include <SDL3/SDL.h>

f64 eTime_GetSeconds() {
        return (f64)SDL_GetPerformanceCounter()/(f64)SDL_GetPerformanceFrequency();
}