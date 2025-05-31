#include "BezierSurfaceC2.h"
#include "Managers/ShaderManager.h"
#include "Core/App.h"

#include <numbers>
#include <Managers/IdManager.h>

std::vector<std::shared_ptr<Point>> BezierSurfaceC2::GetUniqueControlPoints()
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

void BezierSurfaceC2::DetectConnectionType()
{
	if (controlPoints[0]->GetId() == controlPoints[GetColumns() - 1]->GetId())
	{
		connectionType = ConnectionType::Columns;
	}
	else if (controlPoints[0]->GetId() == controlPoints[(GetRows() - 1) * GetColumns()]->GetId())
	{
		connectionType = ConnectionType::Rows;
	}
	else
	{
		connectionType = ConnectionType::Flat;
	}
}

void BezierSurfaceC2::SetupControlPoints(Algebra::Vector4 position, float width, float height)
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
		float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(GetColumns() - 3);

		Algebra::Vector4 startingPosition = position - Algebra::Vector4(0.f, 0.f, height / 2.f);

		for (int i = 0; i < GetRows(); ++i)
		{
			for (int j = 0; j < GetColumns() - 3; ++j)
			{
				Algebra::Vector4 heightOffset = Algebra::Vector4(0.f, 0.f, i * dHeight);
				Algebra::Vector4 radiusOffset = Algebra::Matrix4::RotationZ(dAngle * j) * Algebra::Vector4(width / 2.f, 0.f, 0.f);

				auto point = std::make_shared<Point>("SurfacePoint_");
				point->GetTranslationComponent()->SetTranslation(startingPosition + heightOffset + radiusOffset);
				controlPoints.push_back(point);
			}
			controlPoints.push_back(controlPoints[i * GetColumns() + 0]);
			controlPoints.push_back(controlPoints[i * GetColumns() + 1]);
			controlPoints.push_back(controlPoints[i * GetColumns() + 2]);
		}
	}
	else if (connectionType == ConnectionType::Rows)
	{
		float dHeight = height / static_cast<float>(GetColumns() - 1);
		float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(GetRows() - 3);

		Algebra::Vector4 startingPosition = position - Algebra::Vector4(height / 2.f, 0.f, 0.f);

		for (int i = 0; i < GetRows() - 3; ++i)
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
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < GetColumns(); ++j)
			{
				controlPoints.push_back(controlPoints[i * GetColumns() + j]);
			}
		}
	}
}

void BezierSurfaceC2::SetupPatches()
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
				points.push_back(controlPoints[(startingI + i) * GetColumns() + startingJ + j]);
			}
		}

		Patch patch;
		patch.SetDeBoorPoints(points);
		patches.push_back(patch);
	}
}

BezierSurfaceC2::BezierSurfaceC2()
	: renderer(VertexDataType::PositionVertexData)
{
	name = "BezierSurfaceC2_" + std::to_string(id);
	color = ColorPalette::Get(Color::Purple);
}

BezierSurfaceC2::BezierSurfaceC2(ConnectionType connectionType, Algebra::Vector4 position, float width, float height, 
	int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), connectionType(connectionType), widthPatches(widthPatches), heightPatches(heightPatches)
{
	name = "BezierSurfaceC2_" + std::to_string(id);
	color = ColorPalette::Get(Color::Purple);

	SetupControlPoints(position, width, height);
	SetupPatches();
	Update();
}

void BezierSurfaceC2::Init()
{
	for (auto point : GetUniqueControlPoints())
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
}

void BezierSurfaceC2::Render()
{
	if (somethingChanged)
	{
		Update();
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

	if (drawDeBoorPolygon)
	{
		for (auto& patch : patches)
		{
			patch.RenderDeBoorPolygon();
		}
	}
	if (drawBernsteinPolygon)
	{
		for (auto& patch : patches)
		{
			patch.RenderBernsteinPolygon();
		}
	}
}

void BezierSurfaceC2::Update()
{
	if (bernsteinPoints.empty())
	{
		for (int i = 0; i < patches.size() * 16; ++i)
		{
			auto point = std::make_shared<Point>();
			bernsteinPoints.push_back(point);
		}
	}

	std::vector<PositionVertexData> vertices;

	static const float A[4][4] = {
		{ 1.f / 6.f, 4.f / 6.f, 1.f / 6.f, 0.f       },
		{ 0.f,       4.f / 6.f, 2.f / 6.f, 0.f       },
		{ 0.f,       2.f / 6.f, 4.f / 6.f, 0.f       },
		{ 0.f,       1.f / 6.f, 4.f / 6.f, 1.f / 6.f }
	};

	for (int patchIndex = 0; patchIndex < patches.size(); ++patchIndex)
	{
		auto& patch = patches[patchIndex];

		std::array<std::array<Algebra::Vector4, 4>, 4> P;
		const auto& wps = patch.GetDeBoorPoints();
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				auto sp = wps[i * 4 + j].lock();
				P[i][j] = sp->GetTranslationComponent()->GetTranslation();
			}
		}

		std::array<std::array<Algebra::Vector4, 4>, 4> Q;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				Q[i][j] = Algebra::Vector4();
				for (int k = 0; k < 4; ++k)
					Q[i][j] += A[i][k] * P[k][j];
			}
		}

		std::vector<Algebra::Vector4> B;
		std::vector<std::weak_ptr<Point>> bPoints;
		B.reserve(16);
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				Algebra::Vector4 b = Algebra::Vector4();
				for (int k = 0; k < 4; ++k)
					b += Q[i][k] * A[j][k];
				bernsteinPoints[patchIndex * 16 + i * 4 + j]->GetTranslationComponent()->SetTranslation(b);
				bPoints.push_back(bernsteinPoints[patchIndex * 16 + i * 4 + j]);
			}
		}

		patch.SetBernsteinPoints(bPoints);
	}

	for (auto& point : bernsteinPoints)
	{
		auto p = point->GetTranslationComponent()->GetTranslation();
		p.w = 1.f;
		vertices.push_back(PositionVertexData{ .Position = p });
	}

	renderer.SetVertices(vertices);
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

std::shared_ptr<BezierSurfaceC2> BezierSurfaceC2::Deserialize(const json& j)
{
	auto surface = std::make_shared<BezierSurfaceC2>();

	surface->id = j.at("id").get<unsigned int>();
	if (j.contains("name"))
	{
		surface->name = j.at("name").get<std::string>();
	}

	const auto& size = j.at("size");
	surface->widthPatches = size.at("u").get<int>() - 3;
	surface->heightPatches = size.at("v").get<int>() - 3;

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
