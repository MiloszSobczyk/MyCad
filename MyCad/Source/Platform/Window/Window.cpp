#include "Window.h"

#include "Platform/OpenGL/OpenGLApi.h"

#include <stdexcept>
#include <iostream>

Window::Window(int width, int height, std::string title)
    : width(width), height(height), title(title)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    nativeWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!nativeWindow)
    {
        throw std::runtime_error("Cannot create window");
    }

    glfwMakeContextCurrent(nativeWindow);

    glfwSetWindowUserPointer(nativeWindow, this);

    HandleResize(width, height);

    glfwSetFramebufferSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height) {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->HandleResize(width, height);
        });

    OpenGLApi::InitGLEW();
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(nativeWindow);
}

void Window::ProcessFrame()
{
    glfwSwapBuffers(nativeWindow);
    glfwPollEvents();
}

void Window::HandleResize(int width, int height)
{
    this->width = width;
    this->height = height;
	OpenGLApi::SetViewport(0, 0, width, height);
}