#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Debug.h>

/**
 * Get CPU Ticks in milliseconds
 * @return CPU Ticks in milliseconds
 */
f32 eTime_GetTicks();


/**
 * Get CPU Ticks in seconds
 * @return CPU Ticks in seconds
 */
f32 eTime_GetTicksPerSecond();

/**
 * High Resolution Time
 * @return High Resolution Time in 64bit float
 */
f64 eTime_GetHighResClock();