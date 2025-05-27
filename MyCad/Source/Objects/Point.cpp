#include "Point.h"
#include "Core/App.h"


Point::Point(std::string name)
    : renderer(VertexDataType::PositionVertexData), 
	  translationComponent(std::make_shared<ObservableTranslationComponent>())
{
    std::vector<PositionVertexData> vertices = {
        {{ -0.5f, -0.5f, -0.5f, 1.0f }},
        {{  0.5f, -0.5f, -0.5f, 1.0f }},
        {{  0.5f,  0.5f, -0.5f, 1.0f }},
        {{ -0.5f,  0.5f, -0.5f, 1.0f }},
        {{ -0.5f, -0.5f,  0.5f, 1.0f }},
        {{  0.5f, -0.5f,  0.5f, 1.0f }},
        {{  0.5f,  0.5f,  0.5f, 1.0f }},
        {{ -0.5f,  0.5f,  0.5f, 1.0f }}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 3, 7,  7, 4, 0,
        1, 5, 6,  6, 2, 1,
        0, 1, 5,  5, 4, 0,
        3, 2, 6,  6, 7, 3
    };
    this->name = name + std::to_string(id);
	renderer.SetVertices(vertices);
	renderer.SetIndices(indices);
}

Point::~Point()
{
    NotifyObservers();
}

void Point::Init()
{
    translationComponent->AddObserver(shared_from_this());
}

void Point::Render()
{
    auto shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);

    shader->Bind();
    shader->SetUniformVec4f("u_color", color);
    shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
    shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
    shader->SetUniformMat4f("u_modelMatrix", GetModelMatrix());

	renderer.Render(GL_TRIANGLES);

    shader->Unbind();
}

void Point::RenderUI()
{
    Shape::RenderUI();
    ImGui::Text("Point Position:");
    auto translation = translationComponent->GetTranslation();
    float axisPos[3] = { translation.x, translation.y, translation.z };

    if (ImGui::InputFloat3(("##PointPos" + std::to_string(id)).c_str(), axisPos))
    {
        translationComponent->SetTranslation(Algebra::Vector4(axisPos[0], axisPos[1], axisPos[2], 1.f));
    }
}

std::shared_ptr<TranslationComponent> Point::GetTranslationComponent()
{
    return translationComponent;
}

Algebra::Matrix4 Point::GetModelMatrix() const
{
    return translationComponent->GetMatrix();
}

void Point::OnNotified()
{
    NotifyObservers();
}

json Point::Serialize() const
{
    auto t = translationComponent->GetTranslation();

    json j;
    j["id"] = static_cast<unsigned int>(id);
    if(!name.empty())
    {
        j["name"] = name;
    }
    j["position"] = {
        { "x", t.x },
        { "y", t.y },
        { "z", t.z },
    };

    return j;
}


std::shared_ptr<Point> Point::Deserialize(const json& j)
{
    auto p = std::make_shared<Point>();

    p->id = j.at("id").get<unsigned int>();
    if (j.contains("name"))
    {
        p->name = j.at("name").get<std::string>();
    }
    else
    {
		p->name = "Point_" + std::to_string(p->id);
    }

    const auto& pos = j.at("position");
    float x = pos.at("x").get<float>();
    float y = pos.at("y").get<float>();
    float z = pos.at("z").get<float>();

    p->GetTranslationComponent()->SetTranslation(Algebra::Vector4(x, y, z, 1.f));

    return p;
}
