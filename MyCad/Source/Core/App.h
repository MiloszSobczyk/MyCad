#pragma once
#include "Utils/Init.h"
#include "Algebra.h"
#include "Core/Window.h"
#include "Core/InfiniteGrid.h"
#include "Engine/Camera.h"
#include "Shapes/Torus.h"
#include <string>

class App 
{
public:
	App();
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();
	void Update();
	void DisplayParameters();

private:
	bool active;
	bool showGrid = false;
	Window window;
	Camera camera;
	Torus torus;
	InfiniteGrid grid;
	Shader defaultShader;

	Algebra::Matrix4 projectionMatrix;
	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;
	Algebra::Vector4 GetMousePoint(float x, float y);
};