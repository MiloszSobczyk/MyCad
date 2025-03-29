#include "Translation.h"

Algebra::Vector4 Translation::HandleInput()
{
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
                direction = Algebra::Vector4(delta.x, -delta.y, 0, 0);
                break;
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
