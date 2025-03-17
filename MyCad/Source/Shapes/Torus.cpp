#include "Torus.h"
#include <imgui/imgui.h>

Algebra::Vector4 Torus::GetPoint(float angleTube, float angleRadius)
{
	return Matrix4::RotationYByDegree(angleRadius) *
		Vector4(majorRadius + minorRadius * cosf(angleTube), minorRadius * sinf(angleTube), 0.f, 1.f);
}

void Torus::GeneratePoints()
{
	std::vector<PositionVertexData> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i < majorSegments; i++)
	{
		for (int j = 0; j < minorSegments; j++)
		{
			PositionVertexData vertex;
			vertex.Position = GetPoint(2 * 3.14f * j / minorSegments, 2 * 3.14f * i / majorSegments);
			vertices.push_back(vertex);
		}
	}

	for (int i = 0; i < majorSegments; i++)
	{
		int iNext = (i + 1) % majorSegments;
		for (int j = 0; j < minorSegments; j++)
		{
			int jNext = (j + 1) % minorSegments;
			indices.push_back(minorSegments * i + j);
			indices.push_back(minorSegments * i + jNext);
			indices.push_back(minorSegments * i + j);
			indices.push_back((minorSegments * iNext) + j);
		}
	}

	renderer.SetVertices(vertices);
	renderer.SetIndices(indices);
}

Torus::Torus()
	: renderer(Position)
{
	GeneratePoints();
}

void Torus::HandleInput()
{
	uint32_t min_value = 3;
	uint32_t max_value = 100;

	bool stateChanged = false;

	stateChanged |= ImGui::SliderFloat("Radius", &majorRadius, 1.f, 100.f);
	stateChanged |= ImGui::SliderFloat("Tube Radius", &minorRadius, 1.f, 50.f);
	stateChanged |= ImGui::SliderScalar("Segments", ImGuiDataType_U32, &majorSegments, &min_value, &max_value);
	stateChanged |= ImGui::SliderScalar("Circle Segments", ImGuiDataType_U32, &minorSegments, &min_value, &max_value);

	if (stateChanged)
	{
		GeneratePoints();
	}
}

void Torus::Render()
{
	renderer.Render(GL_LINES);
}
