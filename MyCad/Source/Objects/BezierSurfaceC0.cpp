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

void BezierSurfaceC0::DetectConnectionType()
{
	if (controlPoints[0]->GetId() == controlPoints[GetColumns() - 1]->GetId())
	{
		connectionType = ConnectionType::Columns;
	}
	else if(controlPoints[0]->GetId() == controlPoints[(GetRows() - 1) * GetColumns()]->GetId())
	{
		connectionType = ConnectionType::Rows;
	}
	else
	{
		connectionType = ConnectionType::Flat;
	}
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

BezierSurfaceC0::BezierSurfaceC0()
	: renderer(VertexDataType::PositionVertexData)
{
	name = "BezierSurfaceC0_" + std::to_string(id);
	color = ColorPalette::Get(Color::Purple);
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
	for (auto& point : controlPoints)
	{
		cp.push_back(json{ {"id", static_cast<unsigned int>(point->GetId())} });
	}
	j["controlPoints"] = cp;
	
	j["size"] = {
		{ "u", GetColumns() },
		{ "v", GetRows() },
	};
	j["samples"] = {
		{ "u", tessLevelU },
		{ "v", tessLevelV },
	};

	return j;
}

std::shared_ptr<BezierSurfaceC0> BezierSurfaceC0::Deserialize(const json& j)
{
	auto surface = std::make_shared<BezierSurfaceC0>();

	surface->id = j.at("id").get<unsigned int>();
	if (j.contains("name"))
	{
		surface->name = j.at("name").get<std::string>();
	}

	const auto& size = j.at("size");
	surface->widthPatches = (size.at("u").get<int>() - 1) / 3;
	surface->heightPatches = (size.at("v").get<int>() - 1) / 3;

	const auto& samples = j.at("samples");
	surface->tessLevelU = samples.at("u").get<int>();
	surface->tessLevelV = samples.at("v").get<int>();

	for (const auto& point : j.at("controlPoints"))
	{
		unsigned int id = point.at("id").get<unsigned int>();
		surface->controlPoints.push_back(dynamic_pointer_cast<Point>(IdManager::GetById(id)));
	}

	surface->DetectConnectionType();
	surface->SetupPatches();
	surface->Update();

	return surface;
}
