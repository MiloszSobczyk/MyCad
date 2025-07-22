#include "App.h"

#include "Utils/Config.h"
#include <Platform/Imgui/ImguiApi.h>

App::App()
	: window(Config::WindowWidth, Config::WindowHeight, "Pierce the Heavens"),
	mainSystem()
{
	ImguiApi::InitImgui(window.GetNativeWindow());
}

void App::Run()
{
	while (!window.ShouldClose())
	{
		ImguiApi::NewFrame();

		mainSystem.Update();
		
		ImguiApi::Render();
		
		window.ProcessFrame();
	}
}