#include "App.h"

#include "Utils/Config.h"
#include <Platform/Imgui/ImguiApi.h>

App::App()
	: m_Window(Config::WindowWidth, Config::WindowHeight, "Pierce the Heavens"),
	m_MainSystem()
{
	ImguiApi::InitImgui(m_Window.GetNativeWindow());
}

void App::Run()
{
	while (!m_Window.ShouldClose())
	{
		ImguiApi::NewFrame();

		m_MainSystem.Update();
		
		ImguiApi::Render();
		
		m_Window.ProcessFrame();
	}
}