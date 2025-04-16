#include "Polyline.h"

Polyline::Polyline()
	: renderer(VertexDataType::PositionVertexData)
{
    name = "Polyline" + std::to_string(id);
}


void Polyline::Render()
{
    if (points.size() > 1)
    {
        std::vector<PositionVertexData> vertices;
        std::vector<unsigned int> indices;

        for (auto it = points.begin(); it != points.end();)
        {
            if (auto point = it->lock())
            {
                //TODO:
                //auto position = point->GetTranslation();
                //position.w = 1.f;
                //vertices.push_back(PositionVertexData{ .Position = position });
                //++it;
            }
            else
            {
                it = points.erase(it);
            }
        }

        for (unsigned int i = 0; i < vertices.size() - 1; ++i)
        {
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        renderer.SetVertices(vertices);
        renderer.SetIndices(indices);
        renderer.Render(GL_LINES);
    }
}


void Polyline::AddPoint(std::shared_ptr<Shape> point)
{
	points.push_back(point);
}
