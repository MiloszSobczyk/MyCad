#pragma once

#include "Operation.h"
#include "Core/Window.h"
#include "Objects/AxisCursor.h"

enum class OperationType
{
	Translation = 0,
	Rotation = 1,
	Scaling = 2,
	SIZE,
};

struct OperationParameters
{
	Window* window;
	AxisCursor* cursor;
	std::vector<Shape *> selected;
};

class OperationFactory
{
	Operation* CreateOperation(OperationType type, OperationMode mode, OperationParameters params);
};