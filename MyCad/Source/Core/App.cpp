#include "App.h"

#include "Engine/Shader.h"
#include "Core/Globals.h"
#include <Engine/Renderer.h>
#include <Core/InfiniteGrid.h>

#include <string>
#include <stdexcept>
#include <iostream>
#include <numbers>
#include <algorithm>

App::App()
	: window(Globals::StartingWidth + Globals::RightInterfaceWidth, Globals::StartingHeight, "Pierce the Heavens"),
	active(true), camera(Algebra::Vector4(0.f, 20.f, -50.f, 1.f), 1.f), showGrid(true), shapes(),
	axisCursor(), appMode(AppMode::Camera), selectedShapes(), translation(&camera), middlePoint()
{
	InitImgui(window.GetWindowPointer());
	viewMatrix = Algebra::Matrix4::Identity();

	shapes.push_back(std::make_shared<Torus>());
	shapes.push_back(std::make_shared<Point>());

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
		//glClear(GL_DEPTH_BUFFER_BIT);

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

	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
	{
		currentOperation = std::make_unique<TranslationAlongAxesOperation>(selectedShapes);
	}
	else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
	{
		currentOperation = std::make_unique<RotationAlongAxesOperation>(selectedShapes);
	}

	if (currentOperation)
	{
		currentOperation->HandleInput();
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
	int currentMode = static_cast<int>(appMode);

	if (ImGui::BeginCombo("AppMode", modeNames[currentMode]))
	{
		for (int i = 0; i < 5; ++i)
		{
			bool isSelected = (currentMode == i);
			if (ImGui::Selectable(modeNames[i], isSelected))
			{
				appMode = static_cast<AppMode>(i);
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	const char* modeNames2[] = { "Local", "MiddlePoint", "AxisCursor" };
	currentMode = static_cast<int>(operationMode);

	if (ImGui::BeginCombo("RotationMode", modeNames2[currentMode]))
	{
		for (int i = 0; i < 3; ++i)
		{
			bool isSelected = (currentMode == i);
			if (ImGui::Selectable(modeNames2[i], isSelected))
			{
				operationMode = static_cast<OperationMode>(i);
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
		int index = 0;

		for (const auto& shape : shapes)
		{
			Shape* shapePtr = shape.get();
			bool isSelected = std::any_of(selectedShapes.begin(), selectedShapes.end(), 
				[&](const std::shared_ptr<Shape>& s) { return s.get() == shapePtr; });
			std::string selectableLabel = shapePtr->GetName() + "##" + std::to_string(index++);

			if (ImGui::Selectable(selectableLabel.c_str(), isSelected))
			{
				if (ctrlPressed)
				{
					if (isSelected)
					{
					}
					else
					{
						selectedShapes.push_back(shape);
					}
				}
				else
				{
					selectedShapes.clear();
					selectedShapes.push_back(shape);
				}
			}
		}
	}

	ImGui::EndChild();

	if (ImGui::Button("Delete Selected"))
	{
		for (auto it = shapes.begin(); it != shapes.end();)
		{
			if (std::find(selectedShapes.begin(), selectedShapes.end(), *it) != selectedShapes.end())
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
		selectedShapes.front()->RenderUI();
	}
}

void App::DisplayAddShapeButtons()
{
	ImGui::Text("Add Shape:");

	//if (ImGui::Button("Add Torus"))
	//{
	//	auto torus = std::make_shared<Torus>();
	//	torus->SetTranslation(axisCursor.GetTranslation());
	//	shapes.push_back(torus);
	//}

	//ImGui::SameLine();

	//if (ImGui::Button("Add Point"))
	//{
	//	auto point = std::make_shared<Point>();
	//	point->SetTranslation(axisCursor.GetTranslation());
	//	shapes.push_back(point);
	//}

	//ImGui::SameLine();

	//if (ImGui::Button("Add Polyline"))
	//{
	//	auto polyline = std::make_shared<Polyline>();

	//	for (const auto& shape : shapes)
	//	{
	//		if (std::find(selectedShapes.begin(), selectedShapes.end(), shape.get()) == selectedShapes.end())
	//			continue;
	//		if (auto point = std::dynamic_pointer_cast<Point>(shape))
	//		{
	//			polyline->AddPoint(point);
	//		}
	//	}

	//	shapes.push_back(polyline);
	//}
}

void App::DisplayAxisCursorControls()
{
	ImGui::Text("Axis Cursor Position:");
	float axisPos[3] = { axisCursor.GetTranslation().x, axisCursor.GetTranslation().y, axisCursor.GetTranslation().z };

	if (ImGui::InputFloat3("##AxisCursorPos", axisPos))
	{
		axisCursor.SetTranslation(Algebra::Vector4(axisPos[0], axisPos[1], axisPos[2], 1.f));
	}
}

Algebra::Vector4 App::ScreenToNDC(float x, float y)
{
	float ndcX = (2.0f * x) / (window.GetWidth() - Globals::RightInterfaceWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * y) / window.GetHeight();

	return Algebra::Vector4(ndcX, ndcY, 0.f, 1.f);
}

void App::GetClickedPoint()
{
	auto screenPos = ImGui::GetMousePos();
	auto ndcPos = ScreenToNDC(screenPos.x, screenPos.y);

	if (std::abs(ndcPos.x) > 1.f || std::abs(ndcPos.y) > 1.f)
	{
		return;
	}

	const float similarityThreshold = 0.02f;
	bool isCtrlPressed = ImGui::GetIO().KeyCtrl;

	//for (const auto& shape : shapes)
	//{
	//	if (auto point = std::dynamic_pointer_cast<Point>(shape))
	//	{
	//		Algebra::Vector4 worldPos(0.f, 0.f, 0.f, 1.f);
	//		Algebra::Matrix4 MVP = projectionMatrix * camera.GetViewMatrix() * point->GetModelMatrix();
	//		Algebra::Vector4 clipPos = MVP * worldPos;

	//		clipPos.z = 0.f;

	//		if (clipPos.w != 0.f)
	//		{
	//			clipPos = clipPos / clipPos.w;
	//		}

	//		if (std::abs(ndcPos.x - clipPos.x) < similarityThreshold &&
	//			std::abs(ndcPos.y - clipPos.y) < similarityThreshold)
	//		{
	//			if (isCtrlPressed)
	//			{
	//				auto it = std::find(selectedShapes.begin(), selectedShapes.end(), shape.get());
	//				if (it != selectedShapes.end())
	//				{
	//					selectedShapes.erase(it);
	//				}
	//				else
	//				{
	//					selectedShapes.push_back(shape);
	//				}
	//			}
	//			else
	//			{
	//				selectedShapes.clear();
	//				selectedShapes.push_back(shape);
	//			}
	//		}
	//	}
	//}
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

		shader->SetUniformVec4f("u_color", Algebra::Vector4(1.f, 0.f, 0.f, 1.f));
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