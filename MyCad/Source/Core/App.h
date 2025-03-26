#pragma once

#include <string>
#include <vector>
#include <array>

#include "Algebra.h"
#include "Utils/Init.h"
#include "Core/Window.h"
#include "Core/InfiniteGrid.h"
#include "Engine/Camera.h"
#include "Objects/Objects.h"

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
	Shader defaultShader;

	std::vector<std::shared_ptr<Shape>> shapes;
	AxisCursor axisCursor;

	Algebra::Matrix4 projectionMatrix;
	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;
	Algebra::Vector4 GetMousePoint(float x, float y);

	AppMode mode;

public:
	App();
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();
	void DisplayParameters();
};