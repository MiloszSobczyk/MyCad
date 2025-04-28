#pragma once

#include "Algebra.h"
#include "Core/InfiniteGrid.h"
#include "Core/Window.h"
#include "Engine/Camera.h"
#include "Managers/ShaderManager.h"
#include "Objects/Objects.h"
#include "Objects/SelectedShapes.h"
#include "Operations/OperationFactory.h"
#include "Utils/Init.h"
#include "Objects/BezierCurveC2.h"

#include <array>
#include <string>
#include <unordered_set>
#include <vector>

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
	std::shared_ptr<SelectedShapes> selectedShapes;
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
	void DisplayAddPointsButton();

	Algebra::Vector4 ScreenToNDC(float x, float y);
	void GetClickedPoint();
};