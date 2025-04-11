#pragma once

#include "Operation.h"
#include "Core/Window.h"
#include "Objects/AxisCursor.h"
#include "Engine/Camera.h"

#include "Translations/Translations.h"
#include "Rotations/Rotations.h"

enum class OperationType
{
	Translation = 0,
	Rotation = 1,
	Scaling = 2,
	Camera = 3,
	SIZE,
};

struct OperationParameters
{
	Window* window;
	Camera* camera;
	AxisCursor* cursor;
	std::vector<std::shared_ptr<Shape>>& selected;
	OperationMode mode;
};

class OperationFactory
{
public:
	static std::unique_ptr<Operation> CreateOperation(OperationType type, OperationParameters& params);
};