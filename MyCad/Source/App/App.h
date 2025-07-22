#pragma once

#include "Platform/Window/Window.h"
#include "ECS/Systems/MainSystem.h"

class App
{
public:
	App();

	void Run();

private:
	Window m_Window;
	MainSystem m_MainSystem;
};