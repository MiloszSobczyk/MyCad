#pragma once

#include "Config.h"
#include "Init.h"

#include <string>

class Window
{
public:
    Window(int width, int height, std::string title);

    inline int GetWidth() const { return this->width; }
    inline int GetViewportWidth() const { return this->width - Config::RightInterfaceWidth; }
    inline int GetHeight() const { return this->height; };
    inline std::string GetTitle() const { return this->title; }
    inline GLFWwindow* GetWindowPointer() const { return this->nativeWindow; }

    bool ShouldClose();
    void ProcessFrame();
    void HandleResize(int width, int height);

private:
    void SetupGLFWFunctions();

    int width;
    int height;
    std::string title;
    GLFWwindow* nativeWindow;
};
