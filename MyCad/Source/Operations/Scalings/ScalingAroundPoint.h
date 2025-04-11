#pragma once

#include "Operations/Operation.h"
#include "Objects/AxisCursor.h"

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

	Algebra::Vector4 CalculateMiddlePoint() const;
	Algebra::Vector4 ResolvePoint() const;

public:
	ScalingAroundPoint(std::vector<std::shared_ptr<Shape>>& selected, std::shared_ptr<AxisCursor>& axisCursor);

	void HandleInput() override;
	void DrawGizmos() override;
};