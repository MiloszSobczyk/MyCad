#pragma once

#pragma once
#include <memory>
#include <Objects/Shape.h>
#include "SelectedShapes.h"
#include <Objects/AxisCursor.h>

enum class ShapeEnum
{
	Point = 0,
	Torus = 1,
	Polyline = 2,
	BezierCurve = 3,
};

class ShapeCreator
{
private:
	SelectedShapes* selectedShapes;
	AxisCursor* cursor;
public:
	ShapeCreator(SelectedShapes* selectedShapes, AxisCursor* cursor) : selectedShapes{ selectedShapes }, cursor{ cursor } {}

	std::shared_ptr<Shape> CreateShape(ShapeEnum shape) const;
	static const std::vector<std::pair<ShapeEnum, std::string>>& GetShapeList();
};