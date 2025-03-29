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
	None,
	Translation,
	Rotation,
	Scaling
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
	std::unordered_set<Shape*> selectedShapes;
	AxisCursor axisCursor;

	Algebra::Matrix4 projectionMatrix;
	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;
	Algebra::Vector4 GetMousePoint(float x, float y);

	AppMode mode;
	Translation translation;

public:
	App();
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();
	void DisplayParameters();

	void ApplyOperation();
};