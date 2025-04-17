#include "Torus.h"

#include <imgui/imgui.h>
#include <numbers>

Algebra::Vector4 Torus::GetPoint(float angleTube, float angleRadius)
{
	return Algebra::Matrix4::RotationY(angleRadius) * 
		Algebra::Vector4(majorRadius + minorRadius * cosf(angleTube), minorRadius * sinf(angleTube), 0.f, 1.f);
}

void Torus::GeneratePoints()
{
	stateChanged = false;
	std::vector<PositionVertexData> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i < majorSegments; i++)
	{
		for (int j = 0; j < minorSegments; j++)
		{
			PositionVertexData vertex;
			vertex.Position = GetPoint(2 * std::numbers::pi_v<float> * j / minorSegments, 
				2 * std::numbers::pi_v<float> * i / majorSegments);
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
	: renderer(VertexDataType::PositionVertexData),
	  scalingComponent(std::make_shared<ScalingComponent>()),
	  rotationComponent(std::make_shared<RotationComponent>()),
	  translationComponent(std::make_shared<TranslationComponent>())
{
	name = "Torus" + std::to_string(id);
	GeneratePoints();
}

void Torus::Render()
{
	if (stateChanged)
	{
		GeneratePoints();
	}

	renderer.Render(GL_LINES);
}

void Torus::RenderUI()
{
	Shape::RenderUI();

	int minValue = 3;
	int maxValue = 100;

	stateChanged |= ImGui::SliderFloat("Major Radius", &majorRadius, 1.f, 100.f);
	stateChanged |= ImGui::SliderFloat("Minor Radius", &minorRadius, 1.f, 50.f);
	stateChanged |= ImGui::SliderScalar("Major Segments", ImGuiDataType_U32, &majorSegments, &minValue, &maxValue);
	stateChanged |= ImGui::SliderScalar("Minor Segments", ImGuiDataType_U32, &minorSegments, &minValue, &maxValue);
}

std::shared_ptr<ScalingComponent> Torus::GetScalingComponent()
{
	return scalingComponent;
}

std::shared_ptr<RotationComponent> Torus::GetRotationComponent()
{
	return rotationComponent;
}

std::shared_ptr<TranslationComponent> Torus::GetTranslationComponent()
{
	return translationComponent;
}

Algebra::Matrix4 Torus::GetModelMatrix() const
{
	return translationComponent->GetMatrix() * 
		rotationComponent->GetMatrix() * scalingComponent->GetMatrix();
}
