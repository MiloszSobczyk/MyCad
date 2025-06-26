#pragma once

#include "Core/Window.h"
#include "Core/Config.h"

class App
{
public:
	App();
	~App();

	void Run();

private:
	Window window;
};