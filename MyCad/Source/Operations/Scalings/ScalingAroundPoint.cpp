#include "ScalingAroundPoint.h"

Algebra::Vector4 ScalingAroundPoint::CalculateMiddlePoint() const
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

Algebra::Vector4 ScalingAroundPoint::ResolvePoint() const
{
    return pointMode == PointMode::AxisCursor ? axisCursor->GetTranslation() : CalculateMiddlePoint();
}

ScalingAroundPoint::ScalingAroundPoint(std::vector<std::shared_ptr<Shape>>& selected, 
	std::shared_ptr<AxisCursor>& axisCursor)
	: Operation(selected), axisCursor(axisCursor), pointMode(PointMode::AxisCursor)
{
}

void ScalingAroundPoint::HandleInput()
{
    static const std::vector<std::pair<PointMode, ImGuiKey>> pointBindingMap = {
        { PointMode::AxisCursor, ImGuiKey_A },
        { PointMode::MiddlePoint, ImGuiKey_P },
    };

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

        if (fabs(delta.y) > FLT_EPSILON)
        {
            float scaleFactor = 1.0f - delta.y * 0.01f;

            Algebra::Vector4 point = ResolvePoint();

            for (const auto& shape : selected)
            {
                shape->ScaleAroundPoint(point, Algebra::Vector4(scaleFactor, scaleFactor, scaleFactor, 0.f));
            }
        }
    }
}

void ScalingAroundPoint::DrawGizmos()
{
}
