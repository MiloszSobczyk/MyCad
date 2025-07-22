#pragma once

#include "Platform/Window/Window.h"
#include "ECS/Systems/SceneSystem.h"

class App
{
public:
	App();

	void Run();

private:
	Window m_Window;
	SceneSystem m_SceneSystem;
};