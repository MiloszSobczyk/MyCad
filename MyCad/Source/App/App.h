#pragma once

#include "Platform/Window/Window.h"
#include "ECS/Systems/MainSystem.h"

class App
{
public:
	App();

	void Run();

private:
	Window window;
	MainSystem mainSystem;
};