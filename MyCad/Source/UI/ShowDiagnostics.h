#pragma once

#include "imgui/imgui.h"


namespace UI 
{
    void ShowFPSWindow()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.3f);

        ImGui::Begin("FPS", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing);

        float fps = ImGui::GetIO().Framerate;
        ImGui::Text("FPS: %.1f", fps);

        ImGui::End();
    }
}