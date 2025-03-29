#include "App.h"

#include "Engine/Shader.h"
#include "Core/Globals.h"
#include <Engine/Renderer.h>
#include <Core/InfiniteGrid.h>

#include <string>
#include <stdexcept>
#include <iostream>
#include <numbers>

App::App()
	: window(Globals::StartingWidth + Globals::RightInterfaceWidth, Globals::StartingHeight, "Pierce the Heavens"), 
	active(true), camera(Algebra::Vector4(0.f, 20.f, -50.f, 1.f), 1.f), defaultShader("resources/shaders/default"), 
	showGrid(true), shapes(), axisCursor(), mode(AppMode::None), selectedShapes()
{
	InitImgui(window.GetWindowPointer());
	viewMatrix = Algebra::Matrix4::Identity();

	shapes.push_back(std::make_shared<Torus>());

	//std::shared_ptr<Point> p1 = std::make_shared<Point>();
	//std::shared_ptr<Point> p2 = std::make_shared<Point>();
	//std::shared_ptr<Point> p3 = std::make_shared<Point>();

	//p2->SetTranslation(Algebra::Vector4(0.f, 0.f, 10.f, 0.f));
	//p3->SetTranslation(Algebra::Vector4(0.f, 0.f, 0.f, 0.f));

	//std::shared_ptr<Polyline> pl = std::make_shared<Polyline>();
	//pl->AddPoint(p1);
	//pl->AddPoint(p2);
	//pl->AddPoint(p3);

	//shapes.push_back(p1);
	//shapes.push_back(p2);
	//shapes.push_back(p3);
	//shapes.push_back(pl);


	HandleResize();
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
	while (active && !window.ShouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//axisCursor.HandleInput();

		HandleInput();

		//shapes[0]->AddTranslation(translation.HandleInput());

		DisplayParameters();
		Render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.ProcessFrame();
	}
}

void App::HandleInput()
{
	if (ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		return;
	}

	camera.HandleInput();
}

void App::HandleResize()
{
	float newWidth = static_cast<float>(window.GetWidth() - Globals::RightInterfaceWidth);
	float newHeight = static_cast<float>(window.GetHeight());
	float aspect = newWidth / newHeight;
	projectionMatrix = Algebra::Matrix4::Projection(aspect, 0.1f, 10000.0f, std::numbers::pi_v<float> / 2.f);
}

void App::DisplayParameters()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoDocking;

	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWidth() - Globals::RightInterfaceWidth), 0.f));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(Globals::RightInterfaceWidth), static_cast<float>(window.GetHeight())));

	ImGui::Begin("Main Menu", nullptr, windowFlags);
	ImGui::Checkbox("Show grid", &showGrid);

	const char* modeNames[] = { "None", "Translation", "Rotation", "Scaling" };
	int currentMode = static_cast<int>(mode);

	if (ImGui::BeginCombo("Mode", modeNames[currentMode]))
	{
		for (int i = 0; i < 4; ++i)
		{
			bool isSelected = (currentMode == i);
			if (ImGui::Selectable(modeNames[i], isSelected))
			{
				mode = static_cast<AppMode>(i);
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Text("Shape Selector");

	ImGui::BeginChild("ShapeList", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);

	if (shapes.empty())
	{
		ImGui::Text("No shapes available.");
	}
	else
	{
		bool ctrlPressed = ImGui::GetIO().KeyCtrl;

		for (const auto& shape : shapes)
		{
			Shape* shapePtr = shape.get();
			std::string label = "Shape " + std::to_string(shape->GetId());
			bool isSelected = selectedShapes.count(shapePtr) > 0;

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				if (ctrlPressed)
				{
					if (isSelected)
						selectedShapes.erase(shapePtr);
					else
						selectedShapes.insert(shapePtr);
				}
				else
				{
					selectedShapes.clear();
					selectedShapes.insert(shapePtr);
				}

				std::cout << "Currently selected shapes: ";
				for (const auto& selectedShape : selectedShapes)
					std::cout << selectedShape->GetId() << " ";
				std::cout << std::endl;
			}
		}
	}

	ImGui::EndChild();



	ImGui::End();
}


void App::DisplaySelector()
{
	if (shapes.empty())
	{
		ImGui::Text("No shapes available.");
	}
	else
	{
		for (size_t i = 0; i < shapes.size(); ++i)
		{
			std::string label = "Shape " + std::to_string(shapes[i]->GetId());

			if (ImGui::Selectable(label.c_str()))
			{
				std::cout << "Selected " << label << std::endl;
			}
		}
	}
}


Algebra::Vector4 App::GetMousePoint(float x, float y)
{
	float screenWidth = static_cast<float>(window.GetWidth());
	float screenHeight = static_cast<float>(window.GetHeight());
	float scale = fminf(screenHeight, screenWidth) - 1.f;

	x = (2.f * x - screenWidth + 1.f) / scale;
	y = (2.f * y - screenHeight + 1.f) / -scale;

	float z = 0;
	float d = x * x + y * y;
	if (d <= 1.f / 2.f)
	{
		z = sqrtf(1 - d);
	}
	else
	{
		z = 1.f / 2.f / sqrtf(d);
	}

	return Algebra::Vector4(x, y, z, 0);
}

void App::Render()
{
	if (showGrid)
	{
		grid.Render(camera.GetViewMatrix(), projectionMatrix, camera.GetPosition());
	}

	defaultShader.Bind();
	defaultShader.SetUniformMat4f("u_viewMatrix", camera.GetViewMatrix());
	defaultShader.SetUniformMat4f("u_projectionMatrix", projectionMatrix);
	defaultShader.SetUniformVec4f("u_color", Algebra::Vector4(0.5f, 0.f, 0.5f, 1.f));

	for (auto shape : shapes)
	{
		auto mat = shape->GetModelMatrix();
		defaultShader.SetUniformMat4f("u_modelMatrix", shape->GetModelMatrix());
		shape->Render();
	}

	defaultShader.Unbind();
}