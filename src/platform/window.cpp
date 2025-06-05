#include "window.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

ee::Window::Window ee::Window::Create(ee::Window::CreateInfo& create_info) {
	
	GLFWwindow* window;

	if (!glfwInit())
		throw std::runtime_error("Failed to initialize window manager!");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, create_info.resizable);

	if (create_info.fullscreen) {
		GLFWmonitor* pmonitor = glfwGetPrimaryMonitor();

		if (create_info.adaptiveres) {
			const GLFWvidmode* mode = glfwGetVideoMode(pmonitor);

			window = glfwCreateWindow(mode->width, mode->height, create_info.title.c_str(), pmonitor, nullptr);
		}
		else {
			window = glfwCreateWindow(create_info.size.w, create_info.size.h, create_info.title.c_str(), pmonitor, nullptr);
		}
	}
	else {
		window = glfwCreateWindow(create_info.size.w, create_info.size.h, create_info.title.c_str(), nullptr, nullptr);
	}
	
	if (!window)
		throw std::runtime_error("Failed to create window!");

	glfwMakeContextCurrent(window);
	
	glfwSwapInterval(create_info.vsync);

	return static_cast<void*>(window);
}

void ee::Window::Destroy(ee::Window::Window window) {
	glfwDestroyWindow(static_cast<GLFWwindow*>(window));
}

bool ee::Window::ShouldClose(ee::Window::Window window) {
	return glfwWindowShouldClose(static_cast<GLFWwindow*>(window));
}

void ee::Window::Update(ee::Window::Window window) {
	glfwSwapBuffers(static_cast<GLFWwindow*>(window));
	glfwPollEvents();
}

ee::IVec2 ee::Window::QuerySize(ee::Window::Window window) {
	ee::IVec2 size{};
	glfwGetWindowSize(static_cast<GLFWwindow*>(window), &size.w, &size.h);
	return size;
}