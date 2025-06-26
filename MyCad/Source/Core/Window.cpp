#include "Utils/Logger.h"
#include "Window.h"

#include <stdexcept>

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

    if (!InitGLEW())
    {
        throw std::runtime_error("Cannot inititate glew");
    }

    SetupGLFWFunctions();
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
    GLCall(glViewport(0, 0, std::max(width, 0), height));
}

void Window::SetupGLFWFunctions()
{
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glPatchParameteri(GL_PATCH_VERTICES, 4));
}
