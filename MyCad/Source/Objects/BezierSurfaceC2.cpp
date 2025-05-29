#include "BezierSurfaceC2.h"
#include "Managers/ShaderManager.h"
#include "Core/App.h"

#include <numbers>
#include <Managers/IdManager.h>

// ADD CHOOSING AXIS FOR CYLINDER
// ADD MESH DRAWING


BezierSurfaceC2::BezierSurfaceC2()
	: renderer(VertexDataType::PositionVertexData)
{
}

void BezierSurfaceC2::InitNormally(std::vector<std::shared_ptr<Point>>& jsonPoints)
{
	isCylinder = false;

	const int columns = widthPatches + 3;
	const int rows = heightPatches + 3;

	controlPoints.reserve(rows * columns);
	controlPoints = jsonPoints;

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
	SetupPolygon();
}

void BezierSurfaceC2::InitAsCylinder(std::vector<std::shared_ptr<Point>>& jsonPoints)
{
	const int columns = widthPatches + 2;
	const int rows = heightPatches + 3;

	controlPoints.reserve(rows * columns);

	for (int i = 0; i < jsonPoints.size(); ++i)
	{
		if ((i - columns) % (columns + 1) != 0)
		{
			controlPoints.push_back(jsonPoints[i]);
		}
	}

	for (int patchIndex = 0; patchIndex < widthPatches * (heightPatches + 3); ++patchIndex)
	{
		std::vector<std::weak_ptr<Point>> points;
		std::vector<std::size_t> indices;

		int startingI = patchIndex / (widthPatches + 3);
		int startingJ = patchIndex % (widthPatches + 3);

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
	SetupPolygon();
}

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
	SetupPolygon();
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


	for (int patchIndex = 0; patchIndex < (widthPatches + 3) * heightPatches; ++patchIndex)
	{
		std::vector<std::weak_ptr<Point>> points;
		std::vector<std::size_t> indices;

		int startingI = patchIndex / (widthPatches + 3);
		int startingJ = patchIndex % (widthPatches + 3);

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
	SetupPolygon();
}

void BezierSurfaceC2::OnNotified()
{
	somethingChanged = true;
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
	ImGui::Checkbox("Draw Bernstein polygon", &drawBernsteinPolygon);
	ImGui::Checkbox("Draw De Boor polygon", &drawDeBoorPolygon);

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
	if (somethingChanged)
	{
		UpdateSurface();
		somethingChanged = false;
	}

	auto shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierSurface);

	shader->Bind();
	shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
	shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
	shader->SetUniformInt("u_tessLevelU", tessLevelU);
	shader->SetUniformInt("u_tessLevelV", tessLevelV);

	renderer.SetPatchParameters(16);

	renderer.Render(GL_PATCHES);

	shader->Unbind();


	shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierSurface2);

	shader->Bind();
	shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
	shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
	shader->SetUniformInt("u_tessLevelU", tessLevelU);
	shader->SetUniformInt("u_tessLevelV", tessLevelV);

	renderer.SetPatchParameters(16);

	renderer.Render(GL_PATCHES);

	shader->Unbind();

	renderer.SetPatchParameters(4);

	if (drawBernsteinPolygon)
	{
		bernsteinPolygon->Render();
	}
	if (drawDeBoorPolygon)
	{
		for (auto& patch : patches)
		{
			patch.Render();
		}
	}
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
		{ 1.f / 6, 4.f / 6.f, 1.f / 6.f, 0.f },
		{ 0.f,     4.f / 6.f, 2.f / 6.f, 0.f },
		{ 0.f,     2.f / 6.f, 4.f / 6.f, 0.f },
		{ 0.f,     1.f / 6.f, 4.f / 6.f, 1.f / 6.f }
	};

	for (int patchIndex = 0; patchIndex < patches.size(); ++patchIndex)
	{
		auto& patch = patches[patchIndex];

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
		std::vector<std::weak_ptr<Point>> bPoints;
		B.reserve(16);
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				Algebra::Vector4 b = Algebra::Vector4();
				for (int k = 0; k < 4; ++k)
					b += Q[i][k] * A[j][k];
				bernsteinPoints[patchIndex * 16 + i * 4 + j]->GetTranslationComponent()->SetTranslation(b);
				bPoints.push_back(bernsteinPoints[patchIndex * 16 + i * 4 + j]);
			}

		patch.AddBernstein(bPoints);
	}

	for (auto& point : bernsteinPoints)
	{
		auto p = point->GetTranslationComponent()->GetTranslation();
		p.w = 1.f;
		vertices.push_back(PositionVertexData{ .Position = p });
	}

	renderer.SetVertices(vertices);

	SetupDeBoorPolygon();
}

bool BezierSurfaceC2::HasDuplicates(const json& controlPointsJson)
{
	std::unordered_set<unsigned int> seen;
	seen.reserve(controlPointsJson.size());

	for (const auto& elem : controlPointsJson)
	{
		unsigned int id = elem.at("id").get<unsigned int>();
		if (!seen.insert(id).second)
		{
			return true;
		}
	}
	return false;
}

std::shared_ptr<Point> BezierSurfaceC2::GetPointAt(int row, int col) const
{
	int columns = widthPatches + 2 + (isCylinder ? 0 : 1);
	return controlPoints[(row * columns + col) % controlPoints.size()];
}

std::shared_ptr<Point> BezierSurfaceC2::GetBernsteinPointAt(int row, int col)
{
	int columns = widthPatches * 3 + (isCylinder ? 0 : 1);

	int patchRow = row / 4;
	int patchCol = col / 4;

	if (col >= 4)
	{
		patchCol = (col + 1) / 4;
	}
	if (row >= 4)
	{
		patchRow = (row + 1) / 4;
	}


	int patchIndex = patchRow * widthPatches + patchCol;

	int inPatchRow = row;
	int inPatchColumn = col;

	if (patchCol != 0)
	{
		inPatchColumn = (col - 1) % 3 + 1;
	}
	if (patchRow != 0)
	{
		inPatchRow = (row - 1) % 3 + 1;
	}

	int index = patchIndex * 16 + inPatchRow * 4 + inPatchColumn;

	return bernsteinPoints[patchIndex * 16 + inPatchRow * 4 + inPatchColumn];
}

void BezierSurfaceC2::SetupPolygon()
{
	std::vector<std::weak_ptr<Point>> points;

	if (!isCylinder)
	{
		const int C = widthPatches + 3;
		const int R = heightPatches + 3;
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

		for (int j = startJ; j < C && j >= 0; j += changeJ)
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
		const int C = widthPatches + 2;
		const int R = heightPatches + 3;
		points.reserve(R * C + R * C);
		std::vector<int> indices;

		for (int i = 0; i < R; ++i)
		{
			for (int j = 0; j < C + 1; ++j)
			{
				points.push_back(GetPointAt(i, j % C));
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

	SetupDeBoorPolygon();
}

void BezierSurfaceC2::SetupDeBoorPolygon()
{
	std::vector<std::weak_ptr<Point>> points2;

	if (!isCylinder)
	{
		const int C = widthPatches * 3 + 1;
		const int R = heightPatches * 3 + 1;
		points2.reserve(R * C + R * C);

		for (int i = 0; i < R; ++i)
		{
			if (i % 2 == 0)
			{
				for (int j = 0; j < C; ++j)
					points2.push_back(GetBernsteinPointAt(i, j));
			}
			else
			{
				for (int j = C - 1; j >= 0; --j)
					points2.push_back(GetBernsteinPointAt(i, j));
			}
		}

		int startJ = widthPatches % 2 == 0 ? C - 1 : 0;
		int changeJ = widthPatches % 2 == 0 ? -1 : 1;

		for (int j = startJ; j < C && j >= 0; j += changeJ)
		{
			if (j % 2 == 1)
			{
				for (int i = 0; i < R; ++i)
					points2.push_back(GetBernsteinPointAt(i, j));
			}
			else
			{
				for (int i = R - 1; i >= 0; --i)
					points2.push_back(GetBernsteinPointAt(i, j));
			}
		}
	}
	else
	{
		const int C = widthPatches * 3;
		const int R = heightPatches * 3 + 1;
		points2.reserve(R * C + R * C);
		std::vector<int> indices;

		for (int i = 0; i < R; ++i)
		{
			for (int j = 0; j < C + 1; ++j)
			{
				points2.push_back(GetBernsteinPointAt(i, j % C));
			}
		}

		//for (int j = 1; j < C; ++j)
		//{
		//	if (j % 2 == 0)
		//	{
		//		for (int i = 0; i < R; ++i)
		//		{
		//			points2.push_back(GetBernsteinPointAt(i, j));
		//		}
		//	}
		//	else
		//	{
		//		for (int i = R - 1; i >= 0; --i)
		//		{
		//			points2.push_back(GetBernsteinPointAt(i, j));
		//		}
		//	}
		//}
	}

	deBoorPolygon = std::make_shared<Polyline>(points2);
	deBoorPolygon->SetColor(ColorPalette::Get(Color::Red));
	deBoorPolygon->InitFromPoints();
}

json BezierSurfaceC2::Serialize() const
{
	json j;

	j["objectType"] = "bezierSurfaceC2";
	j["id"] = static_cast<unsigned int>(id);
	if (!name.empty())
	{
		j["name"] = name;
	}

	json cp = json::array();
	int rows = heightPatches + 3;
	int cols = widthPatches + 3;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			int jCole = isCylinder ? j % (widthPatches + 2) : j;
			auto point = GetPointAt(i, jCole);
			cp.push_back(json{ {"id", static_cast<unsigned int>(point->GetId())} });
		}
	}
	j["controlPoints"] = cp;
	j["size"] = {
		{ "u", cols },
		{ "v", rows },
	};
	j["samples"] = {
		{ "u", tessLevelU },
		{ "v", tessLevelV },
	};

	return j;
}

std::shared_ptr<BezierSurfaceC2> BezierSurfaceC2::Deserialize(const json& j)
{
	auto surf = std::make_shared<BezierSurfaceC2>();

	surf->id = j.at("id").get<unsigned int>();
	if (j.contains("name"))
	{
		surf->name = j.at("name").get<std::string>();
	}

	const auto& size = j.at("size");
	surf->widthPatches = size.at("u").get<int>() - 3;
	surf->heightPatches = size.at("v").get<int>() - 3;

	const auto& samples = j.at("samples");
	surf->tessLevelU = samples.at("u").get<int>();
	surf->tessLevelV = samples.at("v").get<int>();

	surf->isCylinder = surf->HasDuplicates(j.at("controlPoints"));

	std::vector<std::shared_ptr<Point>> points;
	for (const auto& cp : j.at("controlPoints"))
	{
		unsigned int pid = cp.at("id").get<unsigned int>();
		auto shape = IdManager::GetById(pid);
		if (auto pt = std::dynamic_pointer_cast<Point>(shape))
		{
			points.push_back(pt);
		}
	}

	if (surf->isCylinder)
	{
		surf->InitAsCylinder(points);
	}
	else
	{
		surf->InitNormally(points);
	}

	return surf;
}
