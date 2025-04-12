#include "RotationAroundPointOperation.h"

Algebra::Vector4 RotationAroundPointOperation::CalculateMiddlePoint() const
{
    if (selected.empty())
    {
        return Algebra::Vector4(0.f, 0.f, 0.f, 1.f);
    }

    Algebra::Vector4 middlePoint(0.f, 0.f, 0.f, 1.f);

    for (const auto& shape : selected)
    {
        middlePoint = middlePoint + shape->GetTranslation();
    }

    middlePoint = middlePoint / selected.size();

    return middlePoint;
}

Algebra::Vector4 RotationAroundPointOperation::ResolvePoint() const
{
    return pointMode == PointMode::AxisCursor ? axisCursor->GetTranslation() : CalculateMiddlePoint();
}

RotationAroundPointOperation::RotationAroundPointOperation(std::vector<std::shared_ptr<Shape>>& selected, std::shared_ptr<AxisCursor>& axisCursor)
	: Operation(selected), axisMode(AxisMode::X), pointMode(PointMode::AxisCursor), axisCursor(axisCursor)
{
}

void RotationAroundPointOperation::HandleInput()
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

    static const std::vector<std::pair<PointMode, ImGuiKey>> pointBindingMap = {
        { PointMode::AxisCursor, ImGuiKey_A },
        { PointMode::MiddlePoint, ImGuiKey_P },
    };

    for (const auto& axisBinding : axisBindingMap)
    {
        if (ImGui::IsKeyPressed(axisBinding.key))
        {
            axisMode = axisBinding.axisMode;
        }
    }

    for (const auto& pointBinding : pointBindingMap)
    {
        if (ImGui::IsKeyPressed(pointBinding.second))
        {
            pointMode = pointBinding.first;
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

            Algebra::Vector4 point = ResolvePoint();

            for (const auto& shape : selected)
            {
                shape->RotateAroundPoint(point, Algebra::Quaternion::CreateFromAxisAngle(axis, angle));
            }
        }
    }
}

void RotationAroundPointOperation::DrawGizmos()
{
}

void RotationAroundPointOperation::RenderUI()
{
    ImGui::Text("Rotation around point");

    ImGui::Text("Current point: %s", pointMode == PointMode::AxisCursor ? "axis cursor" : "middle point");
}
