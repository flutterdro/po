#include "window.h"


Window::Window(int width, int height, char const* title) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef DEBUG_
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		throw std::runtime_error("Failed to create a glfw window\n");
	}
}

auto Window::get_width() const noexcept
	-> int {
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	return width;
}

auto Window::get_height() const noexcept
	-> int {
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	return height;
}

auto Window::get_wwidth() const noexcept
	-> int {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}
auto Window::get_wheight() const noexcept
	-> int {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

auto Window::make_context_current() noexcept
	-> void {
	glfwMakeContextCurrent(window);
}

auto Window::data()
	-> GLFWwindow* {
	return window;
}

auto Window::update()
	-> void {
	glfwSwapBuffers(window);
    glfwWaitEvents();
}

auto Window::is_open()
	-> bool {
	return !glfwWindowShouldClose(window);
}

Window::~Window() {
	glfwDestroyWindow(window);
}