#include "time.hpp"

#include <chrono>
#include <Windows.h>

double ee::Time::GetCurrentTimeInSeconds() {
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

double ee::Time::GetCurrentTimeInMillseconds() {
	return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

double ee::Time::GetCurrentTimeInMicroseconds() {
	return std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

double ee::Time::GetCurrentTimeInNanoseconds() {
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

/**
Sleeps for n milliseconds
*/
void ee::Time::Sleep(double time) {
	::Sleep(static_cast<DWORD>(time));
}