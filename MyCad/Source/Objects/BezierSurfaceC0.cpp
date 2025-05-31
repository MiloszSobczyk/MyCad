#include "BezierSurfaceC0.h"
#include "Managers/ShaderManager.h"
#include "Managers/IdManager.h"
#include "Core/App.h"

#include <numbers>

std::vector<std::shared_ptr<Point>> BezierSurfaceC0::GetUniqueControlPoints()
{
	std::vector<std::shared_ptr<Point>> unique;
	unique.reserve(controlPoints.size());

	std::unordered_set<int> seenIds;
	seenIds.reserve(controlPoints.size());

	for (const auto& p : controlPoints)
	{
		if (!p)
			continue;

		int id = p->GetId();
		if (seenIds.insert(id).second)
		{
			unique.push_back(p);
		}
	}

	return unique;
}

BezierSurfaceC0::BezierSurfaceC0()
	: renderer(VertexDataType::PositionVertexData)
{
	name = "BezierSurfaceC0_" + std::to_string(id);
	color = ColorPalette::Get(Color::Purple);
}

void BezierSurfaceC0::SetupControlPoints(Algebra::Vector4 position, float width, float height)
{
	controlPoints.reserve(GetRows() * GetColumns());

	if (connectionType == ConnectionType::Flat)
	{
		float dx = width / static_cast<float>(GetColumns() - 1);
		float dy = height / static_cast<float>(GetRows() - 1);

		Algebra::Vector4 startingPosition = position - Algebra::Vector4(width / 2.f, height / 2.f, 0.f);

		for (int i = 0; i < GetRows(); ++i)
		{
			for (int j = 0; j < GetColumns(); ++j)
			{
				auto heightOffset = Algebra::Vector4(0.f, i * dy, 0.f);
				auto widthOffset = Algebra::Vector4(j * dx, 0.f, 0.f);

				auto point = std::make_shared<Point>("SurfacePoint_");
				point->GetTranslationComponent()->SetTranslation(startingPosition + widthOffset + heightOffset);

				controlPoints.push_back(point);
			}
		}
	}
	else if (connectionType == ConnectionType::Columns)
	{
		float dHeight = height / static_cast<float>(GetRows() - 1);
		float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(GetColumns() - 1);

		Algebra::Vector4 startingPosition = position - Algebra::Vector4(0.f, 0.f, height / 2.f);

		for (int i = 0; i < GetRows(); ++i)
		{
			for (int j = 0; j < GetColumns() - 1; ++j)
			{
				Algebra::Vector4 heightOffset = Algebra::Vector4(0.f, 0.f, i * dHeight);
				Algebra::Vector4 radiusOffset = Algebra::Matrix4::RotationZ(dAngle * j) * Algebra::Vector4(width / 2.f, 0.f, 0.f);

				auto point = std::make_shared<Point>("SurfacePoint_");
				point->GetTranslationComponent()->SetTranslation(startingPosition + heightOffset + radiusOffset);
				controlPoints.push_back(point);
			}
			controlPoints.push_back(controlPoints[i * GetColumns()]);
		}
	}
	else if (connectionType == ConnectionType::Rows)
	{
		float dHeight = height / static_cast<float>(GetColumns() - 1);
		float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(GetRows() - 1);

		Algebra::Vector4 startingPosition = position - Algebra::Vector4(height / 2.f, 0.f, 0.f);

		for (int i = 0; i < GetRows() - 1; ++i)
		{
			for (int j = 0; j < GetColumns(); ++j)
			{
				Algebra::Vector4 radiusOffset = Algebra::Matrix4::RotationX(dAngle * i) * Algebra::Vector4(0.f, width / 2.f, 0.f);
				Algebra::Vector4 heightOffset = Algebra::Vector4(j * dHeight, 0.f, 0.f);

				auto point = std::make_shared<Point>("SurfacePoint_");
				point->GetTranslationComponent()->SetTranslation(startingPosition + heightOffset + radiusOffset);
				controlPoints.push_back(point);
			}
		}

		for (int j = 0; j < GetColumns(); ++j)
		{
			controlPoints.push_back(controlPoints[j]);
		}
	}
}

void BezierSurfaceC0::SetupPatches()
{
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
				points.push_back(controlPoints[(startingI * 3 + i) * GetColumns() + startingJ * 3 + j]);
			}
		}

		Patch patch;
		patch.SetBernsteinPoints(points);
		patches.push_back(patch);
	}
}

BezierSurfaceC0::BezierSurfaceC0(ConnectionType connectionType, Algebra::Vector4 position, float width, float height, 
	int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), connectionType(connectionType), widthPatches(widthPatches), heightPatches(heightPatches)
{
	name = "BezierSurfaceC0_" + std::to_string(id);
	color = ColorPalette::Get(Color::Purple);

	SetupControlPoints(position, width, height);
	SetupPatches();

	Update();
}

//void BezierSurfaceC0::InitNormally(std::vector<std::shared_ptr<Point>>& jsonPoints)
//{
//	isCylinder = false;
//
//	const int columns = widthPatches * 3 + 1;
//	const int rows = heightPatches * 3 + 1;
//
//	controlPoints.reserve(rows * columns);
//	controlPoints = jsonPoints;
//
//	for (int patchIndex = 0; patchIndex < widthPatches * heightPatches; ++patchIndex)
//	{
//		std::vector<std::weak_ptr<Point>> points;
//		std::vector<std::size_t> indices;
//
//		int startingI = patchIndex / widthPatches;
//		int startingJ = patchIndex % widthPatches;
//
//		for (int i = 0; i < 4; ++i)
//		{
//			for (int j = 0; j < 4; ++j)
//			{
//				points.push_back(controlPoints[(startingI * 3 + i) * columns + startingJ * 3 + j]);
//			}
//		}
//
//		patches.push_back(Patch(points));
//	}
//
//	Update();
//	SetupPolygon();
//}
//
//void BezierSurfaceC0::InitAsCylinder(std::vector<std::shared_ptr<Point>>& jsonPoints)
//{
//	const int columns = widthPatches * 3;
//	const int rows = heightPatches * 3 + 1;
//
//	controlPoints.reserve(rows * columns);
//
//	for (int i = 0; i < jsonPoints.size(); ++i)
//	{
//		if ((i - columns) % (columns + 1) != 0)
//		{
//			controlPoints.push_back(jsonPoints[i]);
//		}
//	}
//
//	for (int patchIndex = 0; patchIndex < widthPatches * heightPatches; ++patchIndex)
//	{
//		std::vector<std::weak_ptr<Point>> points;
//		std::vector<std::size_t> indices;
//
//		int startingI = patchIndex / widthPatches;
//		int startingJ = patchIndex % widthPatches;
//
//		for (int i = 0; i < 4; ++i)
//		{
//			for (int j = 0; j < 4; ++j)
//			{
//				int index = (startingI * 3 + i) * (columns + 1) + (startingJ * 3 + j) % (columns + 1);
//				points.push_back(jsonPoints[index]);
//			}
//		}
//
//		patches.push_back(Patch(points));
//	}
//
//	Update();
//	SetupPolygon();
//}

void BezierSurfaceC0::Init()
{
	for (auto point : GetUniqueControlPoints())
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
}

void BezierSurfaceC0::Render()
{
	if (somethingChanged)
	{
		somethingChanged = false;
		Update();
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
	
	renderer.Render(GL_PATCHES);
	
	shader->Unbind();

	renderer.SetPatchParameters(4);

	if (drawBernsteinPolygon)
	{
		for (auto& patch : patches)
		{
			patch.RenderBernsteinPolygon();
		}
	}
}

void BezierSurfaceC0::Update()
{
	std::vector<PositionVertexData> vertices;

	for (auto& patch : patches)
	{
		for (auto weakPoint : patch.GetBernsteinPoints())
		{
			if (auto sharedPoint = weakPoint.lock())
			{
				auto position = sharedPoint->GetTranslationComponent()->GetTranslation();
				position.w = 1.f;
				vertices.push_back(PositionVertexData{ .Position = position });
			}
		}
	}

	renderer.SetVertices(vertices);
}

// 0 - no duplicates
// 1 - duplicates in each row
// 2 - duplicates in last column
int BezierSurfaceC0::HasDuplicates(const json& controlPointsJson)
{
	int columns = widthPatches * 3 + 1;
	int rows = heightPatches * 3 + 1;

	auto id1 = controlPointsJson[0].at("id").get<unsigned int>();
	auto id2 = controlPointsJson[columns - 1].at("id").get<unsigned int>();
	auto id3 = controlPointsJson[controlPointsJson.size() - columns].at("id").get<unsigned int>();
	if (id1 == id2)
	{
		return 1;
	}
	else if (id1 == id3)
	{
		return 2;
	}
	return 0;
}

std::vector<unsigned int> BezierSurfaceC0::ProcessPoints(std::vector<unsigned int> ids1, int connectionType)
{
	if (connectionType != 2)
	{
		return ids1;
	}

	int temp = widthPatches;
	widthPatches = heightPatches;
	heightPatches = temp;

	int columns = widthPatches * 3 + 1;
	int rows = heightPatches * 3 + 1;

	std::vector<unsigned int> ids;

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			ids.push_back(ids1[j * rows + i]);
		}
	}

	return ids;
}


json BezierSurfaceC0::Serialize() const
{
	json j;

	j["objectType"] = "bezierSurfaceC0";
	j["id"] = static_cast<unsigned int>(id);
	if (!name.empty())
	{
		j["name"] = name;
	}

	json cp = json::array();
	int rows = heightPatches * 3 + 1;
	int cols = widthPatches * 3 + 1;

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{

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

std::shared_ptr<BezierSurfaceC0> BezierSurfaceC0::Deserialize(const json& j)
{
	auto surf = std::make_shared<BezierSurfaceC0>();

	surf->id = j.at("id").get<unsigned int>();
	if (j.contains("name"))
	{
		surf->name = j.at("name").get<std::string>();
	}

	const auto& size = j.at("size");
	surf->widthPatches = (size.at("u").get<int>() - 1) / 3;
	surf->heightPatches = (size.at("v").get<int>() - 1) / 3;

	const auto& samples = j.at("samples");
	surf->tessLevelU = samples.at("u").get<int>();
	surf->tessLevelV = samples.at("v").get<int>();

	auto controlPointsJson = j.at("controlPoints");

	int connectionType1 = surf->HasDuplicates(controlPointsJson);

	std::vector<unsigned int> ids;

	for (const auto& cp : j.at("controlPoints"))
	{
		unsigned int pid = cp.at("id").get<unsigned int>();
		ids.push_back(pid);
	}

	ids = surf->ProcessPoints(ids, connectionType1);

	std::vector<std::shared_ptr<Point>> points;

	for (const auto& id : ids)
	{
		points.push_back(dynamic_pointer_cast<Point>(IdManager::GetById(id)));
	}

	return surf;
}
