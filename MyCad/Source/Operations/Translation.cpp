#include "Translation.h"

Translation::Translation(Camera* camera)
    : camera(camera), state(TranslationState::Camera)
{
}

Algebra::Vector4 Translation::HandleInput()
{
    if (ImGui::IsKeyPressed(ImGuiKey_X))
    {
        state = TranslationState::X;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Y))
    {
        state = TranslationState::Y;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Z))
    {
        state = TranslationState::Z;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_C))
    {
        state = TranslationState::Camera;
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            Algebra::Vector4 direction(0, 0, 0, 0);

            switch (state)
            {
            case TranslationState::X:
                direction = Algebra::Vector4(delta.x, 0, 0, 0);
                break;
            case TranslationState::Y:
                direction = Algebra::Vector4(0, delta.y, 0, 0);
                break;
            case TranslationState::Z:
                direction = Algebra::Vector4(0, 0, -delta.y, 0);
                break;
            case TranslationState::Camera:
            {
                Algebra::Vector4 right = camera->GetRightVector();
                Algebra::Vector4 up = camera->GetUpVector();
                direction = right * delta.x + up * (-delta.y);
                break;
            }
            }

            if (direction.Length() > 0)
            {
                direction = direction.Normalize();
            }

            return direction;
        }
    }

    return Algebra::Vector4();
}
