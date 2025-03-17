#include "Window.h"
#include "utils/GlCall.h"
#include <stdexcept>
#include "App.h"
#include "Globals.h"

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

    HandleResize(width, height);

	glfwSetFramebufferSizeCallback(nativeWindow, [](GLFWwindow* window, int width, int height) {
		Window* newWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		newWindow->HandleResize(width, height);
        });

    if (!InitGLEW())
    {
        throw std::runtime_error("Cannot inititate glew");
    }

    SetupGLFWFunctions();
}

int Window::GetWidth()
{
    return width;
}

int Window::GetHeight()
{
    return height;
}

std::string Window::GetTitle()
{
    return title;
}

GLFWwindow* Window::GetWindowPointer()
{
    return nativeWindow;
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
    GLCall(glViewport(0, 0, std::max(width - Globals::RightInterfaceWidth, 0), height));
}

void Window::SetupGLFWFunctions()
{
    GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}
