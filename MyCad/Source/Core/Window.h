#pragma once
#include "utils/Initialization.h"
#include <string>

class Window
{
private:
	int width;
	int height;
	std::string title;

	GLFWwindow* nativeWindow;

	void SetupGLFWFunctions();

public:
	Window(int width, int height, std::string title);

	int GetWidth();
	int GetHeight();
	std::string GetTitle();
	GLFWwindow* GetWindowPointer();

	bool ShouldClose();
	void ProcessFrame();
	void HandleResize(int width, int height);
};