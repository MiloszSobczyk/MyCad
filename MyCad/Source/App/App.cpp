#include "App.h"

#include "Utils/Config.h"
#include "Utils/Init.h"

App::App()
	: window(Config::WindowWidth + Config::RightInterfaceWidth, Config::WindowHeight, "Pierce the Heavens")
{
	InitImgui(window.GetNativeWindow());
	InitRender();
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