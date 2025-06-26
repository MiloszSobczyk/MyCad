#pragma once

#include "Operations/Operation.h"
#include "Objects/SelectedShapes.h"

#include <imgui/imgui.h>
#include <array>


class RotationAroundAxesOperation : public Operation
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
	RotationAroundAxesOperation(std::shared_ptr<SelectedShapes> selected);

	void HandleInput() override;
	void DrawGizmos() override;
	void RenderUI() override;
};