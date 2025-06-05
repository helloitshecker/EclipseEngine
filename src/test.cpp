#include <platform/window.hpp>
#include <core/time.hpp>
#include <core/io.hpp>

#include <GLFW/glfw3.h>

double delta{};

int main() {
	ee::Window::CreateInfo window_create_info{};
	window_create_info.title = "Eclipse Engine";
	window_create_info.size.w = 800;
	window_create_info.size.h = 600;
	window_create_info.fullscreen = false;
	window_create_info.resizable = true;
	window_create_info.adaptiveres = false;
	window_create_info.vsync = true;

	ee::Window::Window window = ee::Window::Create(window_create_info);

	double oldtime = glfwGetTime();

	while (!ee::Window::ShouldClose(window)) {
		//delta = ee::Time::GetDelta();
		double newtime = glfwGetTime();
		delta = newtime - oldtime;
		oldtime = newtime;

		ee::Out::Print("\rCurrent FPS: {}  ", 1/delta);

		ee::Window::Update(window);
	}
}