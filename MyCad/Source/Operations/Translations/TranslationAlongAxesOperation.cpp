#include "TranslationAlongAxesOperation.h"


TranslationAlongAxesOperation::TranslationAlongAxesOperation(std::shared_ptr<SelectedShapes> selected)
	: Operation(selected), axisMode(AxisMode::X)
{
}

void TranslationAlongAxesOperation::HandleInput()
{
    static const std::vector<std::pair<AxisMode, ImGuiKey>> axisModeMap = {
        { AxisMode::X, ImGuiKey_X },
        { AxisMode::Y, ImGuiKey_Y },
        { AxisMode::Z, ImGuiKey_Z }
    };

	for (const auto& axis : axisModeMap)
	{
		if (ImGui::IsKeyPressed(axis.second))
		{
			axisMode = axis.first;
		}
	}

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            auto normDelta = Algebra::Vector4(delta.x, -delta.y, 0.f, 0.f).Normalize();
            Algebra::Vector4 translation(0, 0, 0, 0);
            translation[static_cast<int>(axisMode)] = axisMode == AxisMode::Y ? normDelta.y : normDelta.x;
            for (const auto& shape : selected->GetSelectedWithType<ITranslation>())
            {
                shape->GetTranslationComponent()->AddTranslation(translation);
            }
        }
    }
}

void TranslationAlongAxesOperation::DrawGizmos()
{
}

void TranslationAlongAxesOperation::RenderUI()
{
    ImGui::Text("Translation along axes");

    ImGui::Text("Current axis: %s",
        axisMode == AxisMode::X ? "X" :
        axisMode == AxisMode::Y ? "Y" :
        axisMode == AxisMode::Z ? "Z" : "");
}