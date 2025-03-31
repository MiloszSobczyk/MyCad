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
#include "Operations/Operations.h"

enum class AppMode
{
	Camera,
	AxisCursor,
	Translation,
	Rotation,
	Scaling
};

enum class RotationMode
{
	Local,
	MiddlePoint,
	Cursor
};

class App 
{
private:
	bool active;
	bool showGrid;
	Window window;
	Camera camera;
	InfiniteGrid grid;

	std::vector<std::shared_ptr<Shape>> shapes;
	std::vector<Shape*> selectedShapes;
	AxisCursor axisCursor;
	Point middlePoint;

	Algebra::Matrix4 projectionMatrix;
	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;

	AppMode appMode;
	RotationMode rotationMode;
	Translation translation;
	Rotation rotation;
	Scaling scaling;

public:
	App();
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();

	void DisplayParameters();
	void DisplayMainMenu();
	void DisplayModeSelection();
	void DisplayShapeSelection();
	void DisplayShapeProperties();
	void DisplayAddShapeButtons();
	void DisplayAxisCursorControls();

	Algebra::Vector4 ScreenToNDC(float x, float y);
	void GetClickedPoint();
};