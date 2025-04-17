#include "ScalingUniformly.h"

ScalingUniformly::ScalingUniformly(std::shared_ptr<SelectedShapes> selected)
	: Operation(selected)
{
}

void ScalingUniformly::HandleInput()
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.y) > FLT_EPSILON)
        {
            float scaleFactor = 1.0f - delta.y * 0.01f;

            for (const auto& shape : selected->GetSelectedWithType<IScaling>())
            {
                shape->GetScalingComponent().AddScaling(Algebra::Vector4(scaleFactor, scaleFactor, scaleFactor, 0.f));
            }
        }
    }
}

void ScalingUniformly::DrawGizmos()
{
}

void ScalingUniformly::RenderUI()
{
    ImGui::Text("Scaling uniformly");
}
