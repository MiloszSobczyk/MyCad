#pragma once

#include "Shape.h"
#include "Engine/Renderer.h"

class Torus : public Shape
{
private:
	Renderer<PositionVertexData> renderer;

	float majorRadius = 15.f;
	float minorRadius = 5.f;
	unsigned int majorSegments = 30;
	unsigned int minorSegments = 50;
	bool stateChanged;

	Algebra::Vector4 GetPoint(float angle, float radius);

	void GeneratePoints();

public:
	Torus();
	void Render() override;
	void RenderUI() override;
};

