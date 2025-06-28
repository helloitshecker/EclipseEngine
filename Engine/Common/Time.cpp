#include "Time.hpp"
#include <SDL3/SDL.h>

inline thread_local Eclipse::Time::TimeData g_time = { 0.0, 0.0 };

void Eclipse::Time::Update() {
    double now = SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
    g_time.delta_time = now - g_time.current_time;
    g_time.current_time = now;
}

const Eclipse::Time::TimeData& Eclipse::Time::Get() {
    return g_time;
}
