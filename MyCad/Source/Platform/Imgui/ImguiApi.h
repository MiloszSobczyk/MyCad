#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ImguiApi
{
// Initialization and Deinitialization
public:
	static bool InitImgui(GLFWwindow* window);
	static void DeinitImgui();

// Frame management
public:
	static void NewFrame();
	static void Render();
};