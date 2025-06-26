#pragma once

#include "Operations/Operation.h"
#include "Objects/AxisCursor.h"
#include "Objects/SelectedShapes.h"

#include <imgui/imgui.h>
#include <array>

class RotationAroundPointOperation : public Operation
{
	enum class AxisMode
	{
		X = 0,
		Y = 1,
		Z = 2,
	};

	enum class PointMode
	{
		AxisCursor,
		MiddlePoint,
	};

private:
	AxisMode axisMode;
	PointMode pointMode;
	std::shared_ptr<AxisCursor> axisCursor;

public:
	RotationAroundPointOperation(std::shared_ptr<SelectedShapes> selected, std::shared_ptr<AxisCursor>& axisCursor);

	void HandleInput() override;
	void DrawGizmos() override;
	void RenderUI() override;
};