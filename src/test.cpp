#include <platform/window.hpp>
#include <core/time.hpp>
#include <core/io.hpp>
#include <graphics/renderer.hpp>
#include <core/memory.hpp>

constexpr ee::FVec4 COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };

double delta{};

int main() {
	ee::Window::CreateInfo window_create_info{};
	window_create_info.title = L"Eclipse Engine Demo";
	window_create_info.size = { 600, 400 };
	window_create_info.position = { USERDEFAULT, USERDEFAULT };
	window_create_info.fullscreen = false;
	window_create_info.resizable = true;
	window_create_info.vsync = true;		
	window_create_info.adaptiveres = false;	// Doesnt Work
	window_create_info.darktheme = true;
	window_create_info.state = ee::Window::WindowState::SHOW;

	ee::Window window(window_create_info);

	ee::Window::CurrentState* window_state = window.GetCurrentStatePtr();

	ee::Renderer renderer{};
	renderer.InitializeRenderer(window);
	renderer.SetVsync(window_create_info.vsync);

	while (!window.ShouldClose()) {
		window.PollEvents();
		
		//ee::Out::Print("\rFPS: {}       ", (int)(1 / delta));

		if (window_state->resize) {
			renderer.SetViewport(window_state->size);
			window_state->resize = false;
		}

		renderer.ClearScreen(COLOR_WHITE);

		delta = ee::Time::GetDelta();

		renderer.SwapBuffers();
	}

	ee::Memory::FreeAll();
}