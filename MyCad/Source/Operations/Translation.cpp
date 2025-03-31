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
    else if (ImGui::IsKeyPressed(ImGuiKey_D))
    {
        state = TranslationState::Camera;
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            auto normDelta = Algebra::Vector4(delta.x, -delta.y, 0.f, 0.f).Normalize();

            Algebra::Vector4 direction(0, 0, 0, 0);

            switch (state)
            {
            case TranslationState::X:
                direction = Algebra::Vector4(normDelta.x, 0, 0, 0);
                break;
            case TranslationState::Y:
                direction = Algebra::Vector4(0, normDelta.y, 0, 0);
                break;
            case TranslationState::Z:
                direction = Algebra::Vector4(0, 0, normDelta.x, 0);
                break;
            case TranslationState::Camera:
            {
                Algebra::Vector4 right = camera->GetRightVector();
                Algebra::Vector4 up = camera->GetUpVector();
                direction = right * normDelta.x + up * (normDelta.y);
                break;
            }
            }

            return direction;
        }
    }

    return Algebra::Vector4();
}
