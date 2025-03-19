#pragma once

#include "Algebra.h"
#include "Engine/Renderer.h"

class AxisCursor
{
private:
	bool visible;

	float size;
	Algebra::Vector4 position;
	Algebra::Vector4 color;

	Renderer<PositionVertexData> renderer;

	std::vector<PositionVertexData> vertices = {
		{{ 0.f, 0.f, 0.f, 1.f }},
		{{ 1.f, 0.f, 0.f, 1.f }},
		{{ 0.f, 1.f, 0.f, 1.f }},
		{{ 0.f, 0.f, 1.f, 1.f }},
	};

	std::vector<unsigned int> indices = {
		0, 1,
		0, 2,
		0, 3,
	};

public:
	AxisCursor();

	void SetPosition(Algebra::Vector4 position);
	void SetSize(float size);
	void SetColor(Algebra::Vector4 color);
	void SetVisible(bool visible);

	Algebra::Vector4 GetColor() { return color; }
	
	void Render();
};