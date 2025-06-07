#include "App.h"

#include "Core/Globals.h"
#include "Engine/Shader.h"
#include "Core/InfiniteGrid.h"
#include "Engine/Renderer.h"
#include "Managers/IdManager.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>

Camera App::camera = Camera(Algebra::Vector4(0.f, 20.f, 50.f, 1.f), 1.f);

Algebra::Matrix4 App::projectionMatrix = Algebra::Matrix4::Projection(1280.f / 720.f, 1.f, 1000.0f, std::numbers::pi_v<float> / 2.f);

Algebra::StereoscopicMatrices App::stereoMatrices = Algebra::Matrix4::StereoscopicProjection(1280.f / 720.f, 1.f, 1000.0f,
	std::numbers::pi_v<float> / 2.f, 0.064f, 5.f);

bool App::useStereoscopy = false;

App::App()
	: window(Globals::StartingWidth + Globals::RightInterfaceWidth, Globals::StartingHeight, "Pierce the Heavens"), 
	axisCursor(std::make_shared<AxisCursor>()), appMode(AppMode::Camera), selectedShapes(std::make_shared<SelectedShapes>()), 
	saveFileBrowser(ImGuiFileBrowserFlags_EnterNewFilename)
{
	InitImgui(window.GetWindowPointer());
	viewMatrix = Algebra::Matrix4::Identity();

	openFileBrowser.SetTitle("Load Scene");
	openFileBrowser.SetTypeFilters({ ".json" });
	saveFileBrowser.SetTitle("Save Scene");
	saveFileBrowser.SetTypeFilters({ ".json" });

	middlePoint.SetColor(ColorPalette::Get(Color::Red));

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
	glEnable(GL_DEPTH_TEST);
	while (active && !window.ShouldClose())
	{
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
	projectionMatrix = Algebra::Matrix4::Projection(aspect, 1.f, 1000.0f, std::numbers::pi_v<float> / 2.f);
	stereoMatrices = Algebra::Matrix4::StereoscopicProjection(aspect, 1.f, 1000.0f,
		std::numbers::pi_v<float> / 2.f, interocularDistance, convergenceDistance);
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
	DisplayStereoscopyParameters();

	DisplaySaveToFile();
	ImGui::SameLine();
	DisplayLoadFromFile();

	DisplaySelectAll();
	ImGui::SameLine();
	DisplayClearShapes();

	if (currentOperation)
	{
		currentOperation->RenderUI();
	}
	DisplayShapeSelection();
	ImGui::Separator();
	DisplayMergePoints();
	ImGui::Separator();
	axisCursor->RenderUI();
	ImGui::Separator();
	DisplayAddShapeButtons();
	ImGui::Separator();
	DisplayFillHolesButton();
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

	static bool CannotDeletePoint = false;

	if (ImGui::Button("Delete Selected"))
	{
		CannotDeletePoint = false;
		for (const auto& shape : shapes)
		{
			if (selectedShapes->IsSelected(shape))
			{
				if (auto selectedPoint = std::dynamic_pointer_cast<Point>(shape))
				{
					if (selectedPoint->IsLocked())
					{
						CannotDeletePoint = true;
						continue;
					}
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

// TODO: one day I will replace this shit
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

	if (ImGui::Button("Add Bezier Surface"))
	{
		bezierParams.showPopup = true;
	}
	if (bezierParams.showPopup)
	{
		DisplayAddSurfacePopup();
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

void App::DisplayAddSurfacePopup()
{
	ImGui::OpenPopup("Edit Bezier Surface");

	ImGui::SetNextWindowPos(ImVec2(window.GetWidth() / 2.f - 50.f, window.GetHeight() / 2.f - 50.f), 
		ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Edit Bezier Surface", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Checkbox("Ensure C2 continuity", &bezierParams.C2);

		static const char* connectionItems[] = { "Flat", "Rows", "Columns" };
		int current = static_cast<int>(bezierParams.connectionType);
		if (ImGui::Combo("Connection Type", &current, connectionItems, IM_ARRAYSIZE(connectionItems))) 
		{
			bezierParams.connectionType = static_cast<ConnectionType>(current);
		}

		if (bezierParams.connectionType == ConnectionType::Flat)
		{
			ImGui::InputFloat("Width", &bezierParams.width);
			if (bezierParams.width < 1.0f) bezierParams.width = 1.0f;
			if (bezierParams.width > 1000.0f) bezierParams.width = 1000.0f;
		}
		else
		{
			ImGui::InputFloat("Diameter", &bezierParams.width);
			if (bezierParams.width < 1.0f) bezierParams.width = 1.0f;
			if (bezierParams.width > 1000.0f) bezierParams.width = 1000.0f;
		}

		ImGui::InputFloat("Height", &bezierParams.height);
		if (bezierParams.height < 1.0f) bezierParams.height = 1.0f;
		if (bezierParams.height > 1000.0f) bezierParams.height = 1000.0f;

		ImGui::InputInt("Width Patches", &bezierParams.widthPatches);
		if (bezierParams.widthPatches < 1) bezierParams.widthPatches = 1;
		if (bezierParams.widthPatches > 100) bezierParams.widthPatches = 10;

		ImGui::InputInt("Height Patches", &bezierParams.heightPatches);
		if (bezierParams.heightPatches < 1) bezierParams.heightPatches = 1;
		if (bezierParams.heightPatches > 100) bezierParams.heightPatches = 10;

		if (ImGui::Button("OK")) 
		{
			ImGui::CloseCurrentPopup();

			if (bezierParams.C2)
			{
				std::shared_ptr<BezierSurfaceC2> bezierSurface = std::make_shared<BezierSurfaceC2>(bezierParams.connectionType,
					axisCursor->GetTranslationComponent()->GetTranslation(), bezierParams.width, bezierParams.height,
					bezierParams.widthPatches, bezierParams.heightPatches);

				bezierSurface->Init();
				shapes.push_back(bezierSurface);

				auto points = bezierSurface->GetUniqueControlPoints();
				for (auto point : points)
				{
					shapes.push_back(point);
				}
			}
			else
			{
				std::shared_ptr<BezierSurfaceC0> bezierSurface = std::make_shared<BezierSurfaceC0>(bezierParams.connectionType, 
					axisCursor->GetTranslationComponent()->GetTranslation(), bezierParams.width, bezierParams.height, 
					bezierParams.widthPatches, bezierParams.heightPatches);

				bezierSurface->Init();
				shapes.push_back(bezierSurface);

				auto points = bezierSurface->GetUniqueControlPoints();
				for (auto point : points)
				{
					shapes.push_back(point);
				}
			}

			bezierParams = BezierSurfaceParams();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) 
		{
			ImGui::CloseCurrentPopup();
			bezierParams = BezierSurfaceParams();
		}
		ImGui::EndPopup();
	}
}

void App::DisplayFillHolesButton()
{
	if (ImGui::Button("Fill holes"))
	{
		selectedShapes->FindEdgeCycles();
	}
}

void App::DisplaySaveToFile()
{
	if (ImGui::Button("Save file"))
		saveFileBrowser.Open();

	saveFileBrowser.Display();

	if (saveFileBrowser.HasSelected())
	{
		const auto path = saveFileBrowser.GetSelected().string();
		json j;
		json points = json::array();
		json geometry = json::array();

		for (auto& shape : shapes)
		{
			if (auto pt = std::dynamic_pointer_cast<Point>(shape))
				points.push_back(pt->Serialize());
			else
				geometry.push_back(shape->Serialize());
		}

		j["points"] = points;
		j["geometry"] = geometry;

		std::ofstream ofs(path);
		ofs << j.dump(4);
		ofs.close();

		saveFileBrowser.ClearSelected();
	}
}

void App::DisplayLoadFromFile()
{
	if (ImGui::Button("Open file"))
		openFileBrowser.Open();

	openFileBrowser.Display();

	if (openFileBrowser.HasSelected())
	{
		const auto path = openFileBrowser.GetSelected().string();

		std::ifstream ifs(path);
		if (!ifs.is_open()) 
		{
			return;
		}

		json j;
		try 
		{
			ifs >> j;
		}
		catch (const json::parse_error& e) 
		{
			return;
		}

		for (const auto& pj : j.at("points"))
		{
			auto p = Point::Deserialize(pj);
			shapes.push_back(p);
			IdManager::RegisterShape(p);
		}

		for (const auto& gj : j.at("geometry"))
		{
			if (gj.at("objectType").get<std::string>() == "torus")
			{
				auto t = Torus::Deserialize(gj);
				shapes.push_back(t);
				IdManager::RegisterShape(t);
			}
			else if (gj.at("objectType").get<std::string>() == "chain")
			{
				auto pl = Polyline::Deserialize(gj);
				shapes.push_back(pl);
				IdManager::RegisterShape(pl);
			}
			else if (gj.at("objectType").get<std::string>() == "bezierC0")
			{
				auto bc = BezierCurve::Deserialize(gj);
				shapes.push_back(bc);
				IdManager::RegisterShape(bc);
			}
			else if (gj.at("objectType").get<std::string>() == "bezierC2")
			{
				auto bc = BezierCurveC2::Deserialize(gj);
				shapes.push_back(bc);
				IdManager::RegisterShape(bc);
			}
			else if (gj.at("objectType").get<std::string>() == "interpolatedC2")
			{
				auto ic = InterpolatingCurve::Deserialize(gj);
				shapes.push_back(ic);
				IdManager::RegisterShape(ic);
			}
			else if (gj.at("objectType").get<std::string>() == "bezierSurfaceC0")
			{
				auto bs = BezierSurfaceC0::Deserialize(gj);
				bs->Init();
				shapes.push_back(bs);
				IdManager::RegisterShape(bs);
			}
			else if (gj.at("objectType").get<std::string>() == "bezierSurfaceC2")
			{
				auto bs = BezierSurfaceC2::Deserialize(gj);
				bs->Init();
				shapes.push_back(bs);
				IdManager::RegisterShape(bs);
			}
		}

		openFileBrowser.ClearSelected();
	}
}

void App::DisplayClearShapes()
{
	if (ImGui::Button("Clear shapes"))
	{
		selectedShapes->Clear();

		for (auto it = shapes.begin(); it != shapes.end(); )
		{
			if (!std::dynamic_pointer_cast<Point>(*it))
				it = shapes.erase(it);
			else
				++it;
		}

		shapes.clear();
		
	}
}

void App::DisplaySelectAll()
{
	if (ImGui::Button("Select all"))
	{
		selectedShapes->Clear();
		for (auto& shape : shapes)
		{
			selectedShapes->AddShape(shape);
		}
	}
}

void App::DisplayStereoscopyParameters()
{
	bool changed = false;

	changed |= ImGui::Checkbox("Use stereoscopy", &useStereoscopy);

	ImGui::Text("Interocular Distance (d)");
	ImGui::PushItemWidth(150.0f);
	changed |= ImGui::SliderFloat("##iod_slider",
		&interocularDistance,
		0.05f,
		1.f,
		"%.3f");
	ImGui::PopItemWidth();

	ImGui::Text("Convergence Distance (c)");
	ImGui::PushItemWidth(150.0f);
	changed |= ImGui::SliderFloat("##convg_slider",
		&convergenceDistance,
		10.0f,
		100.f,
		"%.2f");
	ImGui::PopItemWidth();

	if (changed)
	{
		HandleResize();
	}
}

void App::DisplayMergePoints()
{
	if (ImGui::Button("Merge points"))
	{
		auto points = selectedShapes->GetSelectedWithType<Point>();
		auto result = selectedShapes->MergePoints();
		if (result.has_value())
		{
			for (const auto& point : points) 
			{
				auto it = std::find(shapes.begin(), shapes.end(), point);
				if (it != shapes.end())
					shapes.erase(it);
			}

			shapes.push_back(result.value());
			result.value()->Init();
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
	if (useStereoscopy)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		projectionMatrix = stereoMatrices.left;
		RenderSingleScene();

		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		projectionMatrix = stereoMatrices.right;
		RenderSingleScene();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDisable(GL_BLEND);
	}
	else
	{
		RenderSingleScene();
	}
}

void App::RenderSingleScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	if (showGrid)
	{
		grid.Render(camera.GetViewMatrix(), projectionMatrix, camera.GetPosition());
	}
	glEnable(GL_DEPTH_TEST);

	if (auto avgPos = selectedShapes->GetAveragePosition())
	{
		middlePoint.GetTranslationComponent()->SetTranslation(avgPos.value());
		middlePoint.Render();
	}

	for (auto shape : shapes)
	{
		shape->Render();
	}

	if (useStereoscopy)
	{
		HandleResize();
	}
	axisCursor->Render();
}
