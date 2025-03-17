#pragma once

#include <string>

#include "Algebra.h"
#include "Window.h"
#include "Globals.h"
#include "Utils/Init.h"
#include "Shapes/Torus.h"
#include "Engine/Shader.h"
#include "Engine/Camera.h"

using namespace Algebra;

class App
{
private:
	bool active;
	Window window;
	Camera camera;
	Torus torus;
	Shader shader;

	Vector4 GetMousePoint(float x, float y);
	Vector4 draggingPoint;

	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;

public:
	App(int windowWidth = Globals::DefaultWindowWidth, int windowHeight = Globals::DefaultWindowHeight, std::string title = "Pierce the Heavens");
	~App();

	void Run();
	void Render();
	void HandleInput();
	void HandleResize();
	void Update();
	void DisplayParameters();
};