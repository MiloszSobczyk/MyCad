#pragma once
#include <Window.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <Vector4.h>
#include "Matrix4.h"

using namespace Algebra;

struct UIValues
{
	float a;
	float b;
	float c;
	float scale;
	float translationX;
	float translationY;
	float translationZ;	
	Matrix4 rotation;
	bool moving;
	int raySize;
	int lightIntensity;
};


class UserInterface
{
private:
	Window* window;
	ImGuiWindowFlags flags;
	float width;
	Vector4 startPos;

	void HandleMouseDrag();

public:
	static UIValues values;

	UserInterface(Window* window);
	
	void Render();
	void RenderComponents();
	Vector4 Project(float x, float y);
};