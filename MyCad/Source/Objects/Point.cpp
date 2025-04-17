#include "Point.h"



Point::Point() 
    : renderer(VertexDataType::PositionVertexData)
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

    name = "Point" + std::to_string(id);
	renderer.SetVertices(vertices);
	renderer.SetIndices(indices);
}

void Point::Render()
{
	renderer.Render(GL_TRIANGLES);
}

void Point::RenderUI()
{
    Shape::RenderUI();
    ImGui::Text("Point Position:");
    auto translation = translationComponent.GetTranslation();
    float axisPos[3] = { translation.x, translation.y, translation.z };

    if (ImGui::InputFloat3(("##PointPos" + std::to_string(id)).c_str(), axisPos))
    {
        translationComponent.SetTranslation(Algebra::Vector4(axisPos[0], axisPos[1], axisPos[2], 1.f));
    }
}

TranslationComponent& Point::GetTranslationComponent()
{
    return translationComponent;
}

Algebra::Matrix4 Point::GetModelMatrix() const
{
    return translationComponent.GetMatrix();
}
