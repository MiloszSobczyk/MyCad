#include "Translation.h"

Algebra::Vector4 Translation::HandleInput()
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            Algebra::Vector4 direction = Algebra::Vector4(delta.x, 0, -delta.y, 0);

            if (direction.Length() > 0)
            {
                direction = direction.Normalize();
            }

            return direction;
        }
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            Algebra::Vector4 direction = Algebra::Vector4(0, -delta.y, 0, 0);

            if (direction.Length() > 0)
            {
                direction = direction.Normalize();
            }

            return direction;
        }
    }

    return Algebra::Vector4();
}
