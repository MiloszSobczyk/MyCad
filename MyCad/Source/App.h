#pragma once

#include "Utils/Init.h"
#include "Window.h"
#include "Ellipsoid.h"

#include <string>
#include <RayCaster.h>
#include <UserInterface.h>

class App
{
private:
	Window window;
	bool active;
	Ellipsoid shape;
	RayCaster rayCaster;
	UserInterface UI;
	int raySize;
	int frameCount;

public:
	App(int windowWidth = 1280, int windowHeight = 720, std::string title = "Pierce the Heavens");
	~App();

	void Run();
	void ResolveRaySize();
};