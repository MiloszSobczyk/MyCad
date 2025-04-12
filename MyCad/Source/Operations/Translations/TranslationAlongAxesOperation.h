#pragma once

#include "Operations/Operation.h"

#include <imgui/imgui.h>
#include <array>


class TranslationAlongAxesOperation : public Operation
{
	enum class AxisMode
	{
		X = 0,
		Y = 1,
		Z = 2,
	};

private:
	AxisMode axisMode;
public:
	TranslationAlongAxesOperation(std::vector<std::shared_ptr<Shape>>& selected);

	void HandleInput() override;
	void DrawGizmos() override;
	void RenderUI() override;
};