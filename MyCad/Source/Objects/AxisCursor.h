#pragma once

#include "Algebra.h"
#include "Engine/Renderer.h"

class AxisCursor
{
private:
	static std::vector<PositionVertexData> vertices;
	static std::vector<unsigned int> indices;

	bool visible;

	float size;
	Algebra::Vector4 position;
	Algebra::Vector4 color;

	Renderer<PositionVertexData> renderer;
public:
	AxisCursor();

	void SetPosition(Algebra::Vector4 position);
	void SetSize(float size);
	void SetColor(Algebra::Vector4 color);
	void SetVisible(bool visible);

	Algebra::Vector4 GetColor() { return color; }
	
	void Render();
};