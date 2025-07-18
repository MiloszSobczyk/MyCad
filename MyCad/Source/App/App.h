#pragma once

#include "Platform/Window.h"
#include "ECS/Systems/MainSystem.h"

class App
{
public:
	App();
	~App();

	void Run();

private:
	Window window;
	MainSystem mainSystem;
};