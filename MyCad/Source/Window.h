#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Window
{
private:
	GLFWwindow* nativeWindow;
	int width, height;
	std::string title;

	void ResizeCallback(GLFWwindow* window, int width, int height);
public:
	Window(int width, int height, std::string title);

	inline GLFWwindow* GetNativeWindow() const { return nativeWindow; }
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
	inline std::string GetTitle() { return title; }

	void Update();
	bool ShouldClose();
};