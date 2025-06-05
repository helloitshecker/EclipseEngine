#include "time.hpp"

#include <chrono>

/**
Returns time in milliseconds
*/
double ee::Time::GetCurrent() {
	return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/**
Returns time in nanoseconds
*/
double ee::Time::GetCurrentHighRes() {
	return std::chrono::duration<double, std::nano>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

std::chrono::high_resolution_clock::time_point oldtime = std::chrono::high_resolution_clock().now();
/**
Returns delta time in seconds
*/
double ee::Time::GetDelta() {
	auto newtime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> returntime = newtime - oldtime;
	oldtime = newtime;
	return returntime.count();
}