#include "Init.h"

#include <iostream>

bool InitImgui(GLFWwindow* window)
{
    bool succesful = true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    succesful &= ImGui_ImplGlfw_InitForOpenGL(window, true);
    succesful &= ImGui_ImplOpenGL3_Init();

    return succesful;
}

bool InitGLEW()
{
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Cannot initialize GLEW" << std::endl;
        return false;
    }

    return true;
}