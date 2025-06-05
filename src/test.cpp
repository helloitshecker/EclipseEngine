#include <platform/window.hpp>
#include <core/time.hpp>
#include <core/io.hpp>
#include <graphics/renderer.hpp>

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

	ee::Window::CurrentState* window_state = window.GetCurrentStatePtr();

	ee::Window::InternalInfo window_internal_info = window.GetInternalInfo();

	ee::Renderer renderer{};
	renderer.InitializeRenderer(window_internal_info);

	while (!window.ShouldClose()) {
		window.PollEvents();
		
		if (window_state->resize) {
			renderer.SetViewport(window_state->size);
			window_state->resize = false;
		}

		renderer.ClearScreen({ 1.0f, 1.0f, 1.0f, 1.0f });

		delta = ee::Time::GetDelta();

		renderer.SwapBuffers();
	}
}