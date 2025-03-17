#pragma once
#include "Engine/Renderer.h"

class Torus
{
private:
	Renderer<PositionVertexData> renderer;

	float majorRadius = 15.f;
	float minorRadius = 5.f;
	unsigned int majorSegments = 30;
	unsigned int minorSegments = 50;

	Algebra::Vector4 GetPoint(float angleTube, float angleRadius);

	void GeneratePoints();

public:
	Torus();
	void HandleInput();
	void Render();
};

