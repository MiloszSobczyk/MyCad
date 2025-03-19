#pragma once

#include <string>
#include <vector>

#include "Algebra.h"
#include "Utils/Init.h"
#include "Core/Window.h"
#include "Core/InfiniteGrid.h"
#include "Engine/Camera.h"
#include "Objects/Objects.h"
#include "Managers/ShapeManager.h"


class App 
{
private:
	bool active;
	bool showGrid;
	Window window;
	Camera camera;
	Shape* torus;
	InfiniteGrid grid;
	Shader defaultShader;
	ShapeManager* shapeManager;

	Algebra::Matrix4 projectionMatrix;
	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;
	Algebra::Vector4 GetMousePoint(float x, float y);

public:
	App();
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();
	void DisplayParameters();

};