#include "BezierSurfaceC2.h"
#include "Managers/ShaderManager.h"
#include "Core/App.h"
#include <numbers>

// ADD REMOVING SINGULAR PATCHES
// ADD CHOOSING AXIS FOR CYLINDER
// ADD PATCH HIGHLIGHTING
// ADD DELETION LOCK FOR POINTS

BezierSurfaceC2::BezierSurfaceC2(Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), heightPatches(heightPatches), isCylinder(false)
{
	name = "BezierSurfaceC2_" + std::to_string(id);

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

		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(9);
		indices.push_back(10);

		if (startingI == 0)
		{
			if (startingJ == 0)
			{
				indices.push_back(0);
			}
			else if (startingJ == widthPatches - 1)
			{
				indices.push_back(3);
			}
			indices.push_back(1);
			indices.push_back(2);
		}
		else if (startingI == heightPatches - 1)
		{
			if (startingJ == 0)
			{
				indices.push_back(12);
			}
			else if (startingJ == widthPatches - 1)
			{
				indices.push_back(15);
			}
			indices.push_back(13);
			indices.push_back(14);
		}

		if (startingJ == 0)
		{
			indices.push_back(4);
			indices.push_back(8);
		}
		else if (startingJ == widthPatches - 1)
		{
			indices.push_back(7);
			indices.push_back(11);
		}

		std::sort(indices.begin(), indices.end());
		patches.push_back(Patch(points, indices));
	}

	UpdateSurface();
}

BezierSurfaceC2::BezierSurfaceC2(Algebra::Vector4 position, int axis, float radius, float height, int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), heightPatches(heightPatches), isCylinder(false)
{
	name = "BezierSurfaceC2_" + std::to_string(id);

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

	//for (auto point : controlPoints)
	//{
	//	auto axisVector = Algebra::Vector4();
	//	axisVector[axis] = 1.f;
	//	auto rotation = Algebra::Quaternion::CreateFromAxisAngle(axisVector, std::numbers::pi_v<float> / 2.f);
	//}

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

		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(9);
		indices.push_back(10);

		if (startingI == 0)
		{
			indices.push_back(1);
			indices.push_back(2);
		}
		else if (startingI == heightPatches - 1)
		{
			indices.push_back(13);
			indices.push_back(14);
		}

		if (startingJ == 0)
		{
			indices.push_back(4);
			indices.push_back(8);
		}
		else if (startingJ == widthPatches - 1)
		{
			indices.push_back(7);
			indices.push_back(11);
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
			for (int idx = 0; idx < patches.size(); ++idx)
			{
				bool selected = std::find(selectedPatches.begin(), selectedPatches.end(), idx) != selectedPatches.end();
				patches[idx].SetColor(
					selected
					? ColorPalette::Get(Color::Teal)
					: ColorPalette::Get(Color::Purple)
				);
			}
		}
	}
}

void BezierSurfaceC2::Render()
{
	for (auto point : controlPoints)
	{
		point->Render();
	}

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

void BezierSurfaceC2::RemovePatch(int index)
{
	if (index < 0 || index >= static_cast<int>(patches.size()))
		return;

	patches.erase(patches.begin() + index);

	for (auto it = selectedPatches.begin(); it != selectedPatches.end(); )
	{
		if (*it == index)
		{
			it = selectedPatches.erase(it);
		}
		else
		{
			if (*it > index)
				--(*it);
			++it;
		}
	}
}
