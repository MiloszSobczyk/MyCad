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
	axisCursor(std::make_shared<AxisCursor>()), appMode(AppMode::Camera), selectedShapes(std::make_shared<SelectedShapes>()), 
	middlePoint()
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

		DisplayParameters();
		Render();

		HandleInput();

		if (operationFactory.OperationUpdated)
		{
			useCursor = false;
			OperationParameters params {
				.camera = std::make_shared<Camera>(camera),
				.cursor = axisCursor,
				.selected = selectedShapes,
			};
			currentOperation = operationFactory.CreateOperation(params);
		}

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

	if (ImGui::IsKeyDown(ImGuiKey_E))
	{
		GetClickedPoint();
	}


	if (ImGui::IsKeyDown(ImGuiKey_Escape))
	{
		useCursor = true;
		currentOperation = nullptr;
	}

	operationFactory.HandleInput();

	if (useCursor)
	{
		axisCursor->HandleInput();
		return;
	}


	if (!currentOperation)
	{
		camera.HandleInput();
	}
	else
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
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Checkbox("Show grid", &showGrid);
	if (currentOperation)
	{
		currentOperation->RenderUI();
	}
	DisplayShapeSelection();
	ImGui::Separator();
	axisCursor->RenderUI();
	ImGui::Separator();
	DisplayAddShapeButtons();
	ImGui::Separator();
	DisplayShapeProperties();
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
			bool isSelected = selectedShapes->IsSelected(shape);
			std::string selectableLabel = shapePtr->GetName() + "##" + std::to_string(index++);

			if (ImGui::Selectable(selectableLabel.c_str(), isSelected))
			{
				if (ctrlPressed)
				{
					selectedShapes->ToggleShape(shape);
				}
				else
				{
					selectedShapes->Clear();
					selectedShapes->AddShape(shape);
				}
			}
		}
	}

	ImGui::EndChild();

	if (ImGui::Button("Delete Selected"))
	{
		auto shapesCopy = shapes;
		for (const auto& shape : shapesCopy)
		{
			if (selectedShapes->IsSelected(shape))
			{
				auto it = std::find(shapes.begin(), shapes.end(), shape);
				if (it != shapes.end())
				{
					shapes.erase(it);
				}
			}
		}
		selectedShapes->Clear();
	}
}

void App::DisplayShapeProperties()
{
	if (selectedShapes->Size() == 1)
	{
		selectedShapes->GetAt(0)->RenderUI();
	}
}

void App::DisplayAddShapeButtons()
{
	//TODO: Add constructors with parameters
	ImGui::Text("Add Shape:");

	if (ImGui::Button("Add Torus"))
	{
		auto torus = std::make_shared<Torus>();
		torus->GetTranslationComponent()->SetTranslation(axisCursor->GetTranslationComponent()->GetTranslation());
		shapes.push_back(torus);
	}

	ImGui::SameLine();

	if (ImGui::Button("Add Point"))
	{
		auto point = std::make_shared<Point>();
		point->GetTranslationComponent()->SetTranslation(axisCursor->GetTranslationComponent()->GetTranslation());
		shapes.push_back(point);
	}

	ImGui::SameLine();

	if (ImGui::Button("Add Polyline"))
	{
		auto polyline = std::make_shared<Polyline>();

		auto selectedPoints = selectedShapes->GetSelectedWithType<Point>();
		for (const auto& point : selectedPoints)
		{
			polyline->AddPoint(point);
		}

		shapes.push_back(polyline);
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

	for (const auto& shape : shapes)
	{
		if (auto point = std::dynamic_pointer_cast<Point>(shape))
		{
			Algebra::Vector4 worldPos(0.f, 0.f, 0.f, 1.f);
			Algebra::Matrix4 MVP = projectionMatrix * camera.GetViewMatrix() * point->GetModelMatrix();
			Algebra::Vector4 clipPos = MVP * worldPos;

			clipPos.z = 0.f;

			if (clipPos.w != 0.f)
			{
				clipPos = clipPos / clipPos.w;
			}

			if (std::abs(ndcPos.x - clipPos.x) < similarityThreshold &&
				std::abs(ndcPos.y - clipPos.y) < similarityThreshold)
			{
				if (isCtrlPressed)
				{
					if (selectedShapes->IsSelected(shape))
					{
						selectedShapes->RemoveShape(shape);
					}
					else
					{
						selectedShapes->AddShape(shape);
					}
				}
				else
				{
					selectedShapes->Clear();
					selectedShapes->AddShape(shape);
				}
			}
		}
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

	if (auto avgPos = selectedShapes->GetAveragePosition())
	{
		Algebra::Vector4 middle = avgPos.value();

		middlePoint.GetTranslationComponent()->SetTranslation(middle);

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
	shaderC->SetUniformMat4f("u_modelMatrix", axisCursor->GetModelMatrix());

	axisCursor->Render();

	shaderC->Unbind();
}