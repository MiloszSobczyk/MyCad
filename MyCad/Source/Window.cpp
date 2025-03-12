#include "Window.h"

#include <stdexcept>
#include <Utils/Logger.h>
#include <Utils/Init.h>

Window::Window(int width, int height, std::string title) 
	: width(width), height(height), title(title)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	nativeWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!nativeWindow)
	{
		throw std::runtime_error("Window creation failed");
	}

	glfwSetWindowUserPointer(nativeWindow, static_cast<void*>(this));
	glfwMakeContextCurrent(nativeWindow);

	ResizeCallback(nativeWindow, width, height);

	glfwSetFramebufferSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height) {
		Window* newWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		newWindow->ResizeCallback(window, width, height);
	});

	if (!InitGLEW())
	{
		throw std::runtime_error("Cannot initialize GLEW");
	}
}

void Window::Update()
{
	glfwSwapBuffers(nativeWindow);
	glfwPollEvents();
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(nativeWindow);
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
	this->width = width;
	this->height = height;
	GLCall(glViewport(0, 0, width, height));
}
