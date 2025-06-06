#include "ScalingAroundPoint.h"

ScalingAroundPoint::ScalingAroundPoint(std::shared_ptr<SelectedShapes> selected, std::shared_ptr<AxisCursor>& axisCursor)
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

    if (auto avgPos = selected->GetAveragePosition())
    {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

            if (fabs(delta.y) > FLT_EPSILON)
            {
                float scaleFactor = 1.0f - delta.y * 0.01f;

                Algebra::Vector4 point = pointMode == PointMode::AxisCursor
                    ? axisCursor->GetTranslationComponent()->GetTranslation() : avgPos.value();

                Algebra::Vector4 scaleVector(scaleFactor, scaleFactor, scaleFactor, 0.f);

                for (const auto& shape : selected->GetSelectedWithType<ITranslation>())
                {
                    auto translationComponent = shape->GetTranslationComponent();
                    translationComponent->SetTranslation(point + (translationComponent->GetTranslation() - point).Scale(scaleVector));
                }
                for (const auto& shape : selected->GetSelectedWithType<IScaling>())
                {
                    auto scalingComponent = shape->GetScalingComponent();
                    scalingComponent->SetScaling(scalingComponent->GetScaling().Scale(scaleVector));
                }
            }
        }
    }
}

void ScalingAroundPoint::DrawGizmos()
{
}

void ScalingAroundPoint::RenderUI()
{
    ImGui::Text("Scaling around point");

    ImGui::Text("Current point: %s", pointMode == PointMode::AxisCursor ? "axis cursor" : "middle point");
}
