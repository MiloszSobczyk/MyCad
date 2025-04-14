#pragma once

#include <string>
#include <vector>
#include <array>
#include <unordered_set>

#include "Algebra.h"

#include "Utils/Init.h"
#include "Core/Window.h"
#include "Core/InfiniteGrid.h"
#include "Engine/Camera.h"
#include "Objects/Objects.h"
#include "Managers/ShaderManager.h"
#include "Operations/OperationFactory.h"
#include "Objects/SelectedShapes.h"

enum class AppMode
{
	Camera,
	AxisCursor,
	Translation,
	Rotation,
	Scaling
};

class App 
{
private:
	bool active;
	bool showGrid;
	bool useCursor = true;
	Window window;
	Camera camera;
	InfiniteGrid grid;

	std::vector<std::shared_ptr<Shape>> shapes;
	std::vector<std::shared_ptr<Shape>> selectedShapes;
	std::shared_ptr<AxisCursor> axisCursor;
	Point middlePoint;
	std::shared_ptr<Operation> currentOperation;

	OperationFactory operationFactory;

	Algebra::Matrix4 projectionMatrix;
	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;

	AppMode appMode;
	OperationMode operationMode;

public:
	App();
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();

	void DisplayParameters();
	void DisplayMainMenu();
	void DisplayShapeSelection();
	void DisplayShapeProperties();
	void DisplayAddShapeButtons();
	void DisplayAxisCursorControls();

	Algebra::Vector4 ScreenToNDC(float x, float y);
	void GetClickedPoint();
};