#pragma once

#include <string>

#include "Utils/Init.h"
#include "Window.h"
#include "UserInterface.h"

class App
{
private:
	Window window;
	bool active;

public:
	App(int windowWidth = 1280, int windowHeight = 720, std::string title = "Pierce the Heavens");
	~App();

	void Run();
};