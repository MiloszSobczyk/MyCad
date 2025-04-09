#pragma once

#include "Operations/Operation.h"

#include <imgui/imgui.h>
#include <array>

enum class AxisMode
{
	X = 0,
	Y = 1,
	Z = 2,
};

class TranslationAlongAxesOperation : public Operation
{
private:
	inline static const std::vector<std::pair<AxisMode, ImGuiKey>> axisModeMap = {
		{ AxisMode::X, ImGuiKey_X },
		{ AxisMode::Y, ImGuiKey_Y },
		{ AxisMode::Z, ImGuiKey_Z }
	};

	AxisMode axisMode;
public:
	TranslationAlongAxesOperation(std::vector<std::shared_ptr<Shape>>& selected);

	void HandleInput() override;
	void DrawGizmos() override;
};