#include "BezierSurfaceC2.h"
#include "Managers/ShaderManager.h"
#include "Core/App.h"
#include <numbers>

// ADD CHOOSING AXIS FOR CYLINDER
// ADD MESH DRAWING

BezierSurfaceC2::BezierSurfaceC2(Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), heightPatches(heightPatches), isCylinder(false)
{
	name = "BezierSurfaceC2_" + std::to_string(id);

	const int columns = widthPatches + 3;
	const int rows = heightPatches + 3;

	float dx = width / static_cast<float>(columns - 1);
	float dy = height / static_cast<float>(rows - 1);

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
				points.push_back(controlPoints[(startingI + i) * columns + startingJ + j]);
			}
		}

		patches.push_back(Patch(points, indices));
	}

	UpdateSurface();
}

BezierSurfaceC2::BezierSurfaceC2(Algebra::Vector4 position, int axis, float radius, float height, int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), heightPatches(heightPatches), isCylinder(true)
{
	name = "BezierSurfaceC2_" + std::to_string(id);

	const int columns = widthPatches + 2;
	const int rows = heightPatches + 3;

	float dHeight = height / static_cast<float>(rows - 1);
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
				int index = (startingI + i) * columns + (startingJ + j) % columns;
				points.push_back(controlPoints[index]);
			}
		}

		std::sort(indices.begin(), indices.end());
		patches.push_back(Patch(points, indices));
	}

	UpdateSurface();
}

void BezierSurfaceC2::OnNotified()
{
	UpdateSurface();
}

void BezierSurfaceC2::Init()
{
	for (auto point : controlPoints)
	{
		point->Init();
		point->AddObserver(shared_from_this());
		point->Lock(shared_from_this());
	}
}

void BezierSurfaceC2::RenderUI()
{
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

void BezierSurfaceC2::UpdateColors()
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

void BezierSurfaceC2::Render()
{
	auto shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierSurface);

	shader->Bind();
	shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
	shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
	shader->SetUniformInt("u_tessLevelU", tessLevelU);
	shader->SetUniformInt("u_tessLevelV", tessLevelV);

	renderer.SetPatchParameters(16);

	renderer.Render(GL_PATCHES);

	renderer.SetPatchParameters(4);

	shader->Unbind();
}

void BezierSurfaceC2::UpdateSurface()
{
	if (bernsteinPoints.empty())
	{
		for (int i = 0; i < patches.size() * 16; ++i)
		{
			auto point = std::make_shared<Point>();
			point->GetTranslationComponent()->SetTranslation(Algebra::Vector4());
			bernsteinPoints.push_back(point);
		}
	}

	std::vector<PositionVertexData> vertices;

	static const float A[4][4] = {
		{1.f / 6, 4.f / 6, 1.f / 6, 0.f},
		{0.f,   4.f / 6, 2.f / 6, 0.f},
		{0.f,   2.f / 6, 4.f / 6, 0.f},
		{0.f,   1.f / 6, 4.f / 6, 1.f / 6}
	};

	for (int patchIndex = 0; patchIndex < widthPatches * heightPatches; ++patchIndex)
	{
		auto patch = patches[patchIndex];

		std::array<std::array<Algebra::Vector4, 4>, 4> P;
		const auto& wps = patch.GetPoints();
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				auto sp = wps[i * 4 + j].lock();
				P[i][j] = sp->GetTranslationComponent()->GetTranslation();
			}

		std::array<std::array<Algebra::Vector4, 4>, 4> Q;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				Q[i][j] = Algebra::Vector4();
				for (int k = 0; k < 4; ++k)
					Q[i][j] += A[i][k] * P[k][j];
			}

		std::vector<Algebra::Vector4> B;
		B.reserve(16);
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				Algebra::Vector4 b = Algebra::Vector4();
				for (int k = 0; k < 4; ++k)
					b += Q[i][k] * A[j][k];
				bernsteinPoints[patchIndex * 16 + i * 4 + j]->GetTranslationComponent()->SetTranslation(b);
			}
	}

	for (auto& point : bernsteinPoints)
	{
		auto p = point->GetTranslationComponent()->GetTranslation();
		p.w = 1.f;
		vertices.push_back(PositionVertexData{ .Position = p });
	}

	renderer.SetVertices(vertices);
}