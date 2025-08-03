#pragma once

#include "MouseButton.h"

#include "imgui/imgui.h"

#include <array>


class Input 
{
public:
    // Keyboard
    bool IsKeyPressed(ImGuiKey key) const { return ImGui::IsKeyDown(key); }
    bool IsKeyJustPressed(ImGuiKey key) const { return ImGui::IsKeyPressed(key, false); }
    bool IsKeyJustReleased(ImGuiKey key) const { return ImGui::IsKeyReleased(key); }

    // Mouse buttons
    bool IsMouseDown(MouseButton button) const { return ImGui::IsMouseDown(static_cast<int>(button)); }
    bool IsMouseJustPressed(MouseButton button) const { return ImGui::IsMouseClicked(static_cast<int>(button)); }
    bool IsMouseJustReleased(MouseButton button) const { return ImGui::IsMouseReleased(static_cast<int>(button)); }

    // Mouse movement
    ImVec2 GetMousePosition() const { return ImGui::GetMousePos(); }
    ImVec2 GetMouseDelta(MouseButton button) const { return ImGui::GetMouseDragDelta(static_cast<int>(button)); }
};
