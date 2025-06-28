#pragma once

#include "Platform/Window.h"

class App
{
public:
	App();
	~App();

	void Run();

private:
	Window window;
};