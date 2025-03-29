#include "Rotation.h"

Algebra::Quaternion Rotation::HandleInput()
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            Algebra::Vector4 axis(0, 0, 0, 0);
            float angle = delta.x * 0.01f;

            switch (state)
            {
            case RotationState::X:
                axis = Algebra::Vector4(1, 0, 0, 0);
                break;
            case RotationState::Y:
                axis = Algebra::Vector4(0, 1, 0, 0);
                break;
            case RotationState::Z:
                axis = Algebra::Vector4(0, 0, 1, 0);
                break;
            case RotationState::Trackball:
                axis = Algebra::Vector4(0, 1, 0, 0);
                break;
            }

            if (axis.Length() > 0)
            {
                axis = axis.Normalize();
                return Algebra::Quaternion::CreateFromAxisAngle(axis, angle);
            }
        }
    }

    return Algebra::Quaternion::Identity();
}
