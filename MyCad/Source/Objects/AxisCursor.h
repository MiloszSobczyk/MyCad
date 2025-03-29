#pragma once

#include "Algebra.h"
#include "Engine/Renderer.h"
#include "Shape.h"

class AxisCursor : public Shape
{
private:
	static std::vector<PositionColorVertexData> vertices;
	static std::vector<unsigned int> indices;

	bool visible;

	Renderer<PositionColorVertexData> renderer;
public:
	AxisCursor();
	
	void Render();
	void HandleInput();
};