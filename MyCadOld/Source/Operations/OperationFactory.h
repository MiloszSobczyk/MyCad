#pragma once

#include "Operation.h"
#include "Core/Window.h"
#include "Objects/AxisCursor.h"
#include "Engine/Camera.h"

#include "Translations/Translations.h"
#include "Rotations/Rotations.h"
#include "Scalings/Scalings.h"

#include <imgui/imgui.h>
#include <Objects/SelectedShapes.h>

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
	std::shared_ptr<Camera> camera;
	std::shared_ptr<AxisCursor> cursor;
	std::shared_ptr<SelectedShapes> selected;
};

class OperationFactory
{
private:
	std::shared_ptr<Operation> lastOperation;
	OperationType operationType;
	ImGuiKey lastKey;
public:
	bool OperationUpdated = false;

	void HandleInput();
	std::shared_ptr<Operation> CreateOperation(OperationParameters& params);
};