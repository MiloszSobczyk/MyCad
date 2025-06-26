#pragma once

#include "Operations/Operation.h"
#include "Objects/AxisCursor.h"
#include "Objects/SelectedShapes.h"

class ScalingAroundPoint : public Operation
{
	enum class PointMode
	{
		AxisCursor,
		MiddlePoint,
	};

private:
	PointMode pointMode;
	std::shared_ptr<AxisCursor> axisCursor;
public:
	ScalingAroundPoint(std::shared_ptr<SelectedShapes> selected, std::shared_ptr<AxisCursor>& axisCursor);

	void HandleInput() override;
	void DrawGizmos() override;
	void RenderUI() override;
};