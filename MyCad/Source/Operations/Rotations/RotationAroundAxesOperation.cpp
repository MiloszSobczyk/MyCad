#include "RotationAroundAxesOperation.h"

RotationAroundAxesOperation::RotationAroundAxesOperation(std::shared_ptr<SelectedShapes> selected)
	: Operation(selected), axisMode(AxisMode::X)
{
}

void RotationAroundAxesOperation::HandleInput()
{
    struct AxisBinding {
        AxisMode axisMode;
        ImGuiKey key;
        Algebra::Vector4 axis;
    };

    static const std::vector<AxisBinding> axisBindingMap = {
        { AxisMode::X, ImGuiKey_X, Algebra::Vector4(1.f, 0.f, 0.f) },
        { AxisMode::Y, ImGuiKey_Y, Algebra::Vector4(0.f, 1.f, 0.f) },
        { AxisMode::Z, ImGuiKey_Z, Algebra::Vector4(0.f, 0.f, 1.f) }
    };

    for (const auto& axisBinding : axisBindingMap)
    {
        if (ImGui::IsKeyPressed(axisBinding.key))
        {
            axisMode = axisBinding.axisMode;
        }
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            float angle = delta.x * 0.01f;
            auto axis = std::find_if(axisBindingMap.begin(), axisBindingMap.end(),
                [this](const AxisBinding& binding) { return binding.axisMode == axisMode; })->axis;

            for (const auto& shape : *selected)
            {
                shape->AddRotation(Algebra::Quaternion::CreateFromAxisAngle(axis, angle));
            }
        }
    }
}

void RotationAroundAxesOperation::DrawGizmos()
{
}

void RotationAroundAxesOperation::RenderUI()
{
    ImGui::Text("Rotation around axes");
    ImGui::Text("Current axis: %s",
        axisMode == AxisMode::X ? "X" :
        axisMode == AxisMode::Y ? "Y" :
        axisMode == AxisMode::Z ? "Z" : "");
}
