#include "Scaling.h"

Algebra::Vector4 Scaling::HandleInput()
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.y) > FLT_EPSILON)
        {
            float scaleFactor = 1.0f - delta.y * 0.01f;

            Algebra::Vector4 scale(scaleFactor, scaleFactor, scaleFactor, 0);

            return scale;
        }
    }

    return Algebra::Vector4(1.0f, 1.0f, 1.0f, 0);
}