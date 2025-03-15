#pragma once

#include <string>

#include "Utils/Init.h"
#include "Window.h"
#include "Globals.h"

class App
{
private:
	Window window;
	bool active;

public:
	App(int windowWidth = Globals::DefaultWindowWidth, int windowHeight = Globals::DefaultWindowHeight, std::string title = "Pierce the Heavens");
	~App();

	void Run();
};