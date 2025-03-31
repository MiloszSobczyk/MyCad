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
	active(true), camera(Algebra::Vector4(0.f, 20.f, -50.f, 1.f), 1.f), showGrid(true), shapes(),
	axisCursor(), mode(AppMode::Camera), selectedShapes(), translation(&camera), middlePoint()
{
	InitImgui(window.GetWindowPointer());
	viewMatrix = Algebra::Matrix4::Identity();

	shapes.push_back(std::make_shared<Torus>());

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

		DisplayParameters();
		Render();

		HandleInput();

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

	if (ImGui::IsKeyPressed(ImGuiKey_C))
	{
		mode = AppMode::Camera;
	}
	else if (ImGui::IsKeyPressed(ImGuiKey_A))
	{
		mode = AppMode::AxisCursor;
	}	
	else if (ImGui::IsKeyPressed(ImGuiKey_T))
	{
		mode = AppMode::Translation;
	}
	else if (ImGui::IsKeyPressed(ImGuiKey_R))
	{
		mode = AppMode::Rotation;
	}
	else if (ImGui::IsKeyPressed(ImGuiKey_S))
	{
		mode = AppMode::Scaling;
	}

	switch (mode)
	{
	case AppMode::Camera:
	{
		camera.HandleInput();
		break;
	}
	case AppMode::AxisCursor:
	{
		auto t = translation.HandleInput();
		axisCursor.AddTranslation(t);
		break;
	}
	case AppMode::Translation:
	{
		auto t = translation.HandleInput();
		for (auto shape : selectedShapes)
		{
			shape->AddTranslation(t);
		}
		break;
	}
	case AppMode::Rotation:
	{
		auto r = rotation.HandleInput();
		for (auto shape : selectedShapes)
		{
			shape->AddRotation(r);
		}
		break;
	}
	case AppMode::Scaling:
	{
		auto s = scaling.HandleInput();
		for (auto shape : selectedShapes)
		{
			shape->AddScaling(s);
		}
		break;
	}
	}
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
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDocking;

	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.GetWidth() - Globals::RightInterfaceWidth), 0.f));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(Globals::RightInterfaceWidth), static_cast<float>(window.GetHeight())));

	ImGui::Begin("Main Menu", nullptr, windowFlags);

	DisplayMainMenu();

	ImGui::End();
}

void App::DisplayMainMenu()
{
	ImGui::Checkbox("Show grid", &showGrid);
	DisplayModeSelection();
	ImGui::Separator();
	DisplayShapeSelection();
	ImGui::Separator();
	DisplayAxisCursorControls();
	ImGui::Separator();
	DisplayAddShapeButtons();
	ImGui::Separator();
	DisplayShapeProperties();
}

void App::DisplayModeSelection()
{
	const char* modeNames[] = { "Camera", "AxisCursor", "Translation", "Rotation", "Scaling" };
	int currentMode = static_cast<int>(mode);

	if (ImGui::BeginCombo("Mode", modeNames[currentMode]))
	{
		for (int i = 0; i < 5; ++i)
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
}

void App::DisplayShapeSelection()
{
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
			bool isSelected = selectedShapes.count(shapePtr) > 0;

			if (ImGui::Selectable(shapePtr->GetName().c_str(), isSelected))
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
			}
		}
	}

	ImGui::EndChild();

	if (ImGui::Button("Delete Selected"))
	{
		for (auto it = shapes.begin(); it != shapes.end();)
		{
			if (selectedShapes.count(it->get()))
			{
				it = shapes.erase(it);
			}
			else
			{
				++it;
			}
		}
		selectedShapes.clear();
	}
}

void App::DisplayShapeProperties()
{
	if (selectedShapes.size() == 1)
	{
		(*selectedShapes.begin())->RenderUI();
	}
}

void App::DisplayAddShapeButtons()
{
	ImGui::Text("Add Shape:");

	if (ImGui::Button("Add Torus"))
	{
		auto torus = std::make_shared<Torus>();
		torus->SetTranslation(axisCursor.GetTranslation());
		shapes.push_back(torus);
	}

	ImGui::SameLine();

	if (ImGui::Button("Add Point"))
	{
		auto point = std::make_shared<Point>();
		point->SetTranslation(axisCursor.GetTranslation());
		shapes.push_back(point);
	}
}

void App::DisplayAxisCursorControls()
{
	ImGui::Text("Axis Cursor Position:");
	static float axisPos[3] = { axisCursor.GetTranslation().x, axisCursor.GetTranslation().y, axisCursor.GetTranslation().z };

	if (ImGui::InputFloat3("##AxisCursorPos", axisPos))
	{
		axisCursor.SetTranslation(Algebra::Vector4(axisPos[0], axisPos[1], axisPos[2], 1.f));
	}
}

void App::Render()
{
	if (showGrid)
	{
		grid.Render(camera.GetViewMatrix(), projectionMatrix, camera.GetPosition());
	}

	auto shader = ShaderManager::GetInstance().GetShader("default");

	shader->Bind();
	shader->SetUniformMat4f("u_viewMatrix", camera.GetViewMatrix());
	shader->SetUniformMat4f("u_projectionMatrix", projectionMatrix);

	if (!selectedShapes.empty())
	{
		Algebra::Vector4 middle;
		for (const auto& selectedShape : selectedShapes)
		{
			middle = middle + selectedShape->GetTranslation();
		}
		middlePoint.SetTranslation(middle / selectedShapes.size());
		
		shader->SetUniformMat4f("u_modelMatrix", middlePoint.GetModelMatrix());
		middlePoint.Render();
	}

	for (auto shape : shapes)
	{
		auto mat = shape->GetModelMatrix();
		shader->SetUniformVec4f("u_color", shape->GetColor());
		shader->SetUniformMat4f("u_modelMatrix", shape->GetModelMatrix());
		shape->Render();
	}

	shader->Unbind();

	auto shaderC = ShaderManager::GetInstance().GetShader("defaultColor");

	shaderC->Bind();
	shaderC->SetUniformMat4f("u_viewMatrix", camera.GetViewMatrix());
	shaderC->SetUniformMat4f("u_projectionMatrix", projectionMatrix);
	shaderC->SetUniformMat4f("u_modelMatrix", axisCursor.GetModelMatrix());

	axisCursor.Render();

	shaderC->Unbind();
}