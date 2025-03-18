#pragma once

#include <string>

#include "Algebra.h"
#include "Utils/Init.h"
#include "Core/Window.h"
#include "Core/InfiniteGrid.h"
#include "Engine/Camera.h"
#include "Objects/Objects.h"

class App 
{
public:
	App();
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();
	void DisplayParameters();

private:
	bool active;
	bool showGrid = true;
	Window window;
	Camera camera;
	Shape* torus;
	InfiniteGrid grid;
	Shader defaultShader;

	Algebra::Matrix4 projectionMatrix;
	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;
	Algebra::Vector4 GetMousePoint(float x, float y);
};