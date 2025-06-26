#include "App.h"

#include "Core/Config.h"


App::App()
	: window(Config::WindowWidth + Config::RightInterfaceWidth, Config::WindowHeight, "Pierce the Heavens")
{
	InitImgui(window.GetWindowPointer());
}

App::~App()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void App::Run()
{
	while (!window.ShouldClose())
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.ProcessFrame();
	}
}