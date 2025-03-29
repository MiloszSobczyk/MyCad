#pragma once

#include "Algebra.h"
#include <imgui/imgui.h>

#pragma once

enum class RotationState
{
    Trackball = 0,
    X = 1,
    Y = 2,
    Z = 3
};


class Rotation
{
private:
    RotationState state;

public:
    inline RotationState GetState() { return state; };
    inline void SetState(RotationState state) { this->state = state; };

    Algebra::Quaternion HandleInput()
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

};
