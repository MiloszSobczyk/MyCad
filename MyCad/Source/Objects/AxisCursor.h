#pragma once

#include "Algebra.h"
#include "Engine/Renderer.h"
#include "Shape.h"

class AxisCursor : public Shape
{
private:
	static std::vector<PositionVertexData> vertices;
	static std::vector<unsigned int> indices;

	bool visible;

	Renderer<PositionVertexData> renderer;
public:
	AxisCursor();
	
	void Render();
	void HandleInput();
};