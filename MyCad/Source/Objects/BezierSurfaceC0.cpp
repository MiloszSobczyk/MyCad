#include "BezierSurfaceC0.h"
#include "Managers/ShaderManager.h"
#include "Core/App.h"

#include <numbers>

// ADD CHOOSING AXIS FOR CYLINDER
// ADD MESH DRAWING
// ADD COLORS
// FIX POLYGON

BezierSurfaceC0::BezierSurfaceC0(Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), heightPatches(heightPatches), isCylinder(false)
{
	name = "BezierSurfaceC0_" + std::to_string(id);

	const int columns = widthPatches * 3 + 1;
	const int rows = heightPatches * 3 + 1;

	float dx = width / static_cast<float>(widthPatches * 3);
	float dy = height / static_cast<float>(heightPatches * 3);

	Algebra::Vector4 startingPosition = position - Algebra::Vector4(width / 2.f, height / 2.f, 0.f);

	controlPoints.reserve(rows * columns);

	// Width is along X axis and height is along Y axis, Z axis is flat
	// storing points by columns then rows, like in matrix
	// 0 1 2 3
	// 4 5 6 7

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			auto heightOffset = Algebra::Vector4(0.f, i * dy, 0.f);
			auto widthOffset = Algebra::Vector4(j * dx, 0.f, 0.f);

			auto pos = startingPosition + widthOffset + heightOffset;

			auto point = std::make_shared<Point>("SurfacePoint_");
			point->GetTranslationComponent()->SetTranslation(startingPosition + widthOffset + heightOffset);

			controlPoints.push_back(point);
		}
	}

	for (int patchIndex = 0; patchIndex < widthPatches * heightPatches; ++patchIndex)
	{
		std::vector<std::weak_ptr<Point>> points;
		std::vector<std::size_t> indices;

		int startingI = patchIndex / widthPatches;
		int startingJ = patchIndex % widthPatches;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				points.push_back(controlPoints[(startingI * 3 + i) * columns + startingJ * 3 + j]);
			}
		}

		patches.push_back(Patch(points, indices));
	}

	UpdateSurface();
	SetupPolygon();
}

BezierSurfaceC0::BezierSurfaceC0(Algebra::Vector4 position, int axis, float radius, float height, int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), heightPatches(heightPatches), isCylinder(true)
{
	name = "BezierSurfaceC0_" + std::to_string(id);

	const int columns = widthPatches * 3;
	const int rows = heightPatches * 3 + 1;

	float dHeight = height / static_cast<float>(heightPatches * 3);
	float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(columns);

	Algebra::Vector4 startingPosition = position - Algebra::Vector4(0.f, 0.f, height / 2.f);

	controlPoints.reserve(rows * columns);

	// Width is along X axis and height is along Y axis, Z axis is flat
	// storing points by columns then rows, like in matrix
	// 0 1 2 3
	// 4 5 6 7

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			Algebra::Vector4 heightOffset = Algebra::Vector4(0.f, 0.f, i * dHeight);
			Algebra::Vector4 radiusOffset = Algebra::Matrix4::RotationZ(dAngle * j) * Algebra::Vector4(radius, 0.f, 0.f);
			
			auto point = std::make_shared<Point>("SurfacePoint_");
			point->GetTranslationComponent()->SetTranslation(startingPosition + heightOffset + radiusOffset);

			controlPoints.push_back(point);
		}
	}

	for (int patchIndex = 0; patchIndex < widthPatches * heightPatches; ++patchIndex)
	{
		std::vector<std::weak_ptr<Point>> points;
		std::vector<std::size_t> indices;

		int startingI = patchIndex / widthPatches;
		int startingJ = patchIndex % widthPatches;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				int index = (startingI * 3 + i) * columns + (startingJ * 3 + j) % columns;
				points.push_back(controlPoints[index]);
			}
		}

		patches.push_back(Patch(points, indices));
	}

	UpdateSurface();
	SetupPolygon();
}

void BezierSurfaceC0::OnNotified()
{
	UpdateSurface();
}

void BezierSurfaceC0::Init()
{
	for (auto point : controlPoints)
	{
		point->Init();
		point->AddObserver(shared_from_this());
		point->Lock(shared_from_this());
	}
}

void BezierSurfaceC0::RenderUI()
{
	ImGui::Checkbox("Draw Bernstein polygon", &drawBernsteinPolygon);

	ImGui::SeparatorText("Tessellation");
	ImGui::SliderInt("Tess Level U", &tessLevelU, 1, 64);
	ImGui::SliderInt("Tess Level V", &tessLevelV, 1, 64);

	ImGui::SeparatorText("Patches");

	if (patches.empty())
	{
		ImGui::Text("No patches available.");
	}
	else
	{
		ImGuiIO& io = ImGui::GetIO();
		bool ctrlHeld = io.KeyCtrl;
		bool changed = false;

		for (int i = 0; i < patches.size(); ++i)
		{
			bool isSelected = std::find(selectedPatches.begin(), selectedPatches.end(), i) != selectedPatches.end();
			std::string label = "Patch " + std::to_string(i) + "##patch" + std::to_string(i);

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				changed = true;
				if (ctrlHeld)
				{
					if (isSelected)
					{
						selectedPatches.erase(
							std::remove(selectedPatches.begin(), selectedPatches.end(), i),
							selectedPatches.end()
						);
					}
					else
					{
						selectedPatches.push_back(i);
					}
				}
				else
				{
					if (isSelected)
					{
						selectedPatches.clear();
					}
					else
					{
						selectedPatches.clear();
						selectedPatches.push_back(i);
					}
				}
			}
		}

		if (changed)
		{
			UpdateColors();
		}
	}
}

void BezierSurfaceC0::UpdateColors()
{
	for (auto& patch : patches)
	{
		patch.SetColor(ColorPalette::Get(Color::Purple));
	}

	for (int idx : selectedPatches)
	{
		if (idx >= 0 && idx < patches.size())
			patches[idx].SetColor(ColorPalette::Get(Color::Teal));
	}
}

void BezierSurfaceC0::Render()
{

	auto shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierSurface);

	shader->Bind();
	shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
	shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
	shader->SetUniformInt("u_tessLevelU", tessLevelU);
	shader->SetUniformInt("u_tessLevelV", tessLevelV);

	//renderer.SetPatchParameters(16);
	
	renderer.Render(GL_PATCHES);

	shader->Unbind();


	shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierSurface2);
	
	shader->Bind();
	shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
	shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
	shader->SetUniformInt("u_tessLevelU", tessLevelU);
	shader->SetUniformInt("u_tessLevelV", tessLevelV);
	
	renderer.SetPatchParameters(16);

	//renderer.Render(GL_PATCHES);
	
	shader->Unbind();

	renderer.SetPatchParameters(4);

	if (drawBernsteinPolygon)
	{
		bernsteinPolygon->Render();
	}
}

void BezierSurfaceC0::UpdateSurface()
{
	std::vector<PositionVertexData> vertices;

	for (auto& patch : patches)
	{
		for (auto wp : patch.GetPoints())
		{
			if (auto sp = wp.lock())
			{
				auto p = sp->GetTranslationComponent()->GetTranslation();
				p.w = 1.f;
				vertices.push_back(PositionVertexData{ .Position = p });
			}
		}
	}

	renderer.SetVertices(vertices);
}

std::shared_ptr<Point> BezierSurfaceC0::GetPointAt(int row, int col)
{
	int columns = widthPatches * 3 + (isCylinder ? 0 : 1);
	return controlPoints[(row * columns + col) % controlPoints.size()];
}

void BezierSurfaceC0::SetupPolygon()
{
	std::vector<std::weak_ptr<Point>> points;

	if (!isCylinder)
	{
		const int C = widthPatches * 3 + 1;
		const int R = heightPatches * 3 + 1;
		points.reserve(R * C + R * C);

		for (int i = 0; i < R; ++i)
		{
			if (i % 2 == 0)
			{
				for (int j = 0; j < C; ++j)
					points.push_back(GetPointAt(i, j));
			}
			else
			{
				for (int j = C - 1; j >= 0; --j)
					points.push_back(GetPointAt(i, j));
			}
		}

		int startJ = widthPatches % 2 == 0 ? C - 1 : 0;
		int changeJ = widthPatches % 2 == 0 ? -1 : 1;

		for (int j = startJ; j < C && j > 0; j += changeJ)
		{
			if (j % 2 == 0)
			{
				for (int i = 0; i < R; ++i)
					points.push_back(GetPointAt(i, j));
			}
			else
			{
				for (int i = R - 1; i >= 0; --i)
					points.push_back(GetPointAt(i, j));
			}
		}
	}
	else 
	{
		const int C = widthPatches * 3;
		const int R = heightPatches * 3 + 1;
		points.reserve(R * C + R * C);
		std::vector<int> indices;

		int columns = widthPatches * 3 + (isCylinder ? 0 : 1);

		for (int i = 0; i < R; ++i)
		{
			for (int j = 0; j < C + 1; ++j)
			{
				points.push_back(GetPointAt(i, j));
			}
		}

		for (int j = 1; j < C; ++j)
		{
			if (j % 2 == 0)
			{
				for (int i = 0; i < R; ++i)
				{
					points.push_back(GetPointAt(i, j));
				}
			}
			else
			{
				for (int i = R - 1; i >= 0; --i)
				{
					points.push_back(GetPointAt(i, j));
				}
			}
		}
	}

	bernsteinPolygon = std::make_shared<Polyline>(points);
	bernsteinPolygon->SetColor(ColorPalette::Get(Color::Teal));
	bernsteinPolygon->InitFromPoints();
}
