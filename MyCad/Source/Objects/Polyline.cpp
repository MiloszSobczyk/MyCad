#include "Polyline.h"

Polyline::Polyline()
	: renderer(VertexDataType::PositionVertexData)
{
}

void Polyline::HandleInput()
{
}

void Polyline::Render()
{
	for (auto it = points.begin(); it != points.end();)
	{
		if (auto point = it->lock())
		{
			point->Render();
			++it;
		}
		else
		{
			points.erase(it);
		}
	}

	if (points.size() > 1)
	{
		std::vector<PositionVertexData> vertices;
		std::vector<unsigned int> indices;

		for (auto it = points.begin(); it != points.end(); ++it)
		{
			if (auto point = it->lock())
			{
				auto position = point->GetModelMatrix() * point->GetPosition();
				vertices.push_back({{ position.x, position.y, position.z, 1.f }});
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

void Polyline::AddPoint(std::shared_ptr<Point> point)
{
	points.push_back(point);
}
