#include "io.hpp"

#ifdef _WIN32
#include <Windows.h>

const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void ee::Out::RawPrint(const std::wstring& str) {
	WriteConsoleW(hConsole, str.c_str(), str.length(), nullptr, nullptr);
}
void ee::Out::RawPrint(const std::string& str) {
	WriteConsoleA(hConsole, str.c_str(), str.length(), nullptr, nullptr);
}
#endif