#include <platform/window.hpp>
#include <core/time.hpp>
#include <core/io.hpp>

double delta{};

int main() {
	ee::Window::CreateInfo window_create_info{};
	window_create_info.title = L"Eclipse Engine Demo";
	window_create_info.size = { 600, 400 };
	window_create_info.position = { USERDEFAULT, USERDEFAULT };
	window_create_info.fullscreen = false;
	window_create_info.resizable = true;
	window_create_info.vsync = true;		// X
	window_create_info.adaptiveres = false;	// X
	window_create_info.darktheme = true;
	window_create_info.state = ee::Window::WindowState::SHOW;

	ee::Window window(window_create_info);


	while (!window.ShouldClose()) {
		window.PollEvents();
	}
}