#include "App.h"
#include <stdexcept>

App::App(int windowWidth, int windowHeight, std::string title)
    : window(windowWidth, windowHeight, title), active(true), shape(1.f, 1.f, 1.f), UI(&window), raySize(2), frameCount(3)
{
    active &= InitImgui(window.GetNativeWindow());
    if (!active)
    {
        throw std::runtime_error("cannot initialize app");
    }
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void App::Run()
{
    while (active && !window.ShouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        ResolveRaySize();
        rayCaster.CalculatePoints(shape, window, raySize);
        rayCaster.Render();

        UI.Render();

        window.Update();
    }
}

void App::ResolveRaySize()
{
    auto values = UserInterface::values;
    if (values.moving)
    {
        raySize = values.raySize;
        frameCount = 3;
        return;
    }
    if (frameCount-- == 0)
    {
        raySize = std::max(raySize / 2, 2);
        frameCount = 3;
    }
}
