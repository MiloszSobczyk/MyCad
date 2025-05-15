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

Camera App::camera = Camera(Algebra::Vector4(0.f, 20.f, -50.f, 1.f), 1.f);
Algebra::Matrix4 App::projectionMatrix = Algebra::Matrix4::Projection(1280 / 720, 0.1f, 10000.0f, std::numbers::pi_v<float> / 2.f);
static bool CannotDeletePoint = false;

App::App()
	: window(Globals::StartingWidth + Globals::RightInterfaceWidth, Globals::StartingHeight, "Pierce the Heavens"),
	active(true), showGrid(true), shapes(),
	axisCursor(std::make_shared<AxisCursor>()), appMode(AppMode::Camera), selectedShapes(std::make_shared<SelectedShapes>()), 
	middlePoint()
{
	InitImgui(window.GetWindowPointer());
	viewMatrix = Algebra::Matrix4::Identity();

	middlePoint.SetColor(Algebra::Vector4(1.0f, 0.f, 0.f, 1.f));

	//auto pointPtr = std::make_shared<Point>();
	//pointPtr->Init();
	//shapes.push_back(pointPtr);

	//auto pointPtr2 = std::make_shared<Point>();
	//pointPtr2->Init();
	//pointPtr2->GetTranslationComponent()->SetTranslation(Algebra::Vector4(0.f, 20.f, 0.f, 1.f));
	//shapes.push_back(pointPtr2);

	//auto pointPtr3 = std::make_shared<Point>();
	//pointPtr3->Init();
	//pointPtr3->GetTranslationComponent()->SetTranslation(Algebra::Vector4(20.f, 20.f, 0.f, 1.f));
	//shapes.push_back(pointPtr3);

	//auto pointPtr4 = std::make_shared<Point>();
	//pointPtr4->Init();
	//pointPtr4->GetTranslationComponent()->SetTranslation(Algebra::Vector4(20.f, 0.f, 0.f, 1.f));
	//shapes.push_back(pointPtr4);

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

	if (selectedShapes->Size() == 1)
	{
		auto selectedShape = selectedShapes->GetAt(0);
		if (auto curve = std::dynamic_pointer_cast<BezierCurveC2>(selectedShape))
		{
			curve->HandleInput();
			return;
		}
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
	DisplayAddPointsButton();
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
		CannotDeletePoint = false;
		for (const auto& shape : shapes)
		{
			if (selectedShapes->IsSelected(shape))
			{
				if (auto selectedPoint = std::dynamic_pointer_cast<Point>(shape))
				{
					CannotDeletePoint = true;
					continue;
				}

				auto it = std::find(shapes.begin(), shapes.end(), shape);
				if (it != shapes.end())
				{
					shapes.erase(it);
				}
			}
		}

		selectedShapes->Clear();
	}

	if (CannotDeletePoint)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot delete point (text to be replaced)");
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
		point->Init();
		shapes.push_back(point);

		auto bezierCurves = selectedShapes->GetSelectedWithType<BezierCurve>();
		for (auto curve : bezierCurves)
		{
			curve->AddPoint(point);
		}
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

	if (ImGui::Button("Add Bezier Curve C0"))
	{
		auto bezierCurve = std::make_shared<BezierCurve>();

		auto selectedPoints = selectedShapes->GetSelectedWithType<Point>();
		for (const auto& point : selectedPoints)
		{
			bezierCurve->AddPoint(point);
		}

		shapes.push_back(bezierCurve);
	}

	if (ImGui::Button("Add Bezier Curve C2"))
	{
		auto bezierCurve = std::make_shared<BezierCurveC2>();

		auto selectedPoints = selectedShapes->GetSelectedWithType<Point>();
		for (const auto& point : selectedPoints)
		{
			bezierCurve->AddPoint(point);
		}

		shapes.push_back(bezierCurve);
	}

	if (ImGui::Button("Add Interpolating Curve"))
	{
		auto bezierCurve = std::make_shared<InterpolatingCurve>();

		auto selectedPoints = selectedShapes->GetSelectedWithType<Point>();
		for (const auto& point : selectedPoints)
		{
			bezierCurve->AddPoint(point);
		}

		shapes.push_back(bezierCurve);
	}

	if (ImGui::Button("Add Bezier Surface C0"))
	{
		auto bezierSurface = std::make_shared<BezierSurfaceC0>(Algebra::Vector4(), false, 20.f, 20.f, 1, 1);
		bezierSurface->Init();
		shapes.push_back(bezierSurface);

		auto points = bezierSurface->GetControlPoints();
		for (auto point : points)
		{
			shapes.push_back(point);
		}
	}
}

void App::DisplayAddPointsButton()
{
	if (ImGui::Button("Add points to polyline/bezier curve"))
	{
		auto points = selectedShapes->GetSelectedWithType<Point>();
		for(auto line : selectedShapes->GetSelectedWithType<Polyline>())
		{
			for (auto point : points)
			{
				line->AddPoint(point);
			}
		}
		for (auto curve : selectedShapes->GetSelectedWithType<BezierCurve>())
		{
			for (auto point : points)
			{
				curve->AddPoint(point);
			}
		}
		for (auto curve : selectedShapes->GetSelectedWithType<BezierCurveC2>())
		{
			for (auto point : points)
			{
				curve->AddPoint(point);
			}
		}
		for (auto curve : selectedShapes->GetSelectedWithType<InterpolatingCurve>())
		{
			for (auto point : points)
			{
				curve->AddPoint(point);
			}
		}
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

	if (auto avgPos = selectedShapes->GetAveragePosition())
	{
		middlePoint.GetTranslationComponent()->SetTranslation(avgPos.value());
		middlePoint.Render();
	}

	for (auto shape : shapes)
	{
		shape->Render();
	}

	axisCursor->Render();
}