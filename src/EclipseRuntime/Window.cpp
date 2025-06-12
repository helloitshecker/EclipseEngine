#include "Window.hpp"
#include <EclipseUtils/Logger.hpp>

using namespace e;

double previous_call_time = 0;

window::window(window::WindowCreateInfo& window_create_info) {
	GLFWmonitor* monitor = nullptr;

	if (!glfwInit()) {
		LOG::SEND("Failed to intialize Window!");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, window_create_info.resizable);
	glfwWindowHint(GLFW_SAMPLES, window_create_info.samples);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	if (window_create_info.fullscreen) {
		monitor = glfwGetPrimaryMonitor();
		if (!monitor) {
			LOG::SEND("Failed to find a display!");
			glfwTerminate();
		}

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (!mode) {
			LOG::SEND("Failed to get video mode!");
			glfwTerminate();
		}

		// If width and height are 0 then use display resolution
		if (window_create_info.width == 0 || window_create_info.height == 0) {
			window_width = mode->width;
			window_height = mode->height;
		}
		// Else use resolution provided by user
		else {
			window_width = window_create_info.width;
			window_height = window_create_info.height;
		}
	}
	else {
		window_width = window_create_info.width;
		window_height = window_create_info.height;
	}

	// Call window create function
	pwindow = glfwCreateWindow(window_width, window_height, window_create_info.title, monitor, nullptr);

	if (!pwindow) {
		LOG::SEND("Window creation failed!");
		glfwTerminate();
	}

	glfwSetFramebufferSizeCallback(pwindow, window_create_info.resize_callback);
	glfwSetCursorPosCallback(pwindow, window_create_info.cursor_pos_callback);

	glfwMakeContextCurrent(pwindow);

	glfwSwapInterval(window_create_info.vsync);
}

void window::update() const {
	glfwSwapBuffers(pwindow);
	glfwPollEvents();
}

bool window::shouldClose() const {
	return glfwWindowShouldClose(pwindow);
}

window::~window() {
	if (!pwindow) {
		glfwTerminate();
	}
}

double window::getTime() const {
	return glfwGetTime();
}

double window::getIntervalToPreviousCall() const {
	double now = getTime();
	double diff = now - previous_call_time;
	previous_call_time = now;
	return diff;
}

et::keystate window::getKeyState(et::key key) const {
	return glfwGetKey(pwindow, key);
}

GLFWglproc window::getProcAddress(const char* procname) {
	return glfwGetProcAddress(procname);
}

void window::setVsync(bool state) const {
	glfwSwapInterval(state);
}