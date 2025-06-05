#ifdef _WIN32
#include "window.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

constexpr wchar_t CLASS_NAME[] = L"EclipseEngine";
const HINSTANCE hInstance = GetModuleHandle(nullptr);
HWND hWnd{};
MSG msg{};
bool shouldclose{};
WINDOWPLACEMENT oldPlacement;
DWORD oldStyle;
bool isFullscreen = false;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		shouldclose = true;
		PostQuitMessage(0);
		break;
	};

	// handle fallback messages
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

ee::Window::Window(ee::Window::CreateInfo& create_info) {

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hWnd = CreateWindowEx(
		0,														// Window Styles
		CLASS_NAME,												// Window Class
		static_cast<LPCWSTR>(create_info.title.c_str()),		// Window title
		WS_OVERLAPPEDWINDOW,									// Window style

		create_info.position.x,									// Position X
		create_info.position.y,									// Position Y
		create_info.size.w,										// Width
		create_info.size.h,										// Height

		nullptr,												// Parent window
		nullptr,												// Menu
		hInstance,												// Instance Handle
		nullptr													// Additional Data
	);

	if (hWnd == nullptr) {
		throw WindowCreationFailed;
	}

	GetWindowPlacement(hWnd, &oldPlacement);
	oldStyle = GetWindowLong(hWnd, GWL_STYLE);

	SetFullscreenState(create_info.fullscreen, create_info.adaptiveres, create_info.size);

	SetTitleBarDarkMode(create_info.darktheme);

	if (!create_info.resizable) {
		LONG style = GetWindowLong(hWnd, GWL_STYLE);
		style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		SetWindowLong(hWnd, GWL_STYLE, style);
	}

	ShowWindow(hWnd, static_cast<int>(create_info.state));
}

void ee::Window::PollEvents() {
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool ee::Window::ShouldClose() {
	return shouldclose;
}

void ee::Window::SetFullscreenState(bool fullscreen, bool adaptiveres, ee::IVec2 res) {
	if (fullscreen) {
		// Save window state
		GetWindowPlacement(hWnd, &oldPlacement);
		oldStyle = GetWindowLong(hWnd, GWL_STYLE);

		// get primary monitor res
		MONITORINFO mi = { sizeof(mi) };
		if (GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
			SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			if (adaptiveres) {
				SetWindowPos(hWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
			}
			else {
				SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
				SetWindowPos(hWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
			}
		}
	}
	else {
		// Restore state
		SetWindowLong(hWnd, GWL_STYLE, oldStyle);
		SetWindowPlacement(hWnd, &oldPlacement);
		SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
	}
}

void ee::Window::SetTitleBarDarkMode(bool mode) {
	const DWORD attribute = DWMWA_USE_IMMERSIVE_DARK_MODE;
	BOOL useDarkMode = mode ? TRUE : FALSE;
	DwmSetWindowAttribute(hWnd, attribute, &useDarkMode, sizeof(useDarkMode));
}

#endif