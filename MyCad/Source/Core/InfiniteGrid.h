#pragma once

#include "Algebra.h"

#include <engine/Shader.h>
#include <engine/Renderer.h>

class InfiniteGrid
{
private:
	Renderer<PositionVertexData> renderer;
public:
	InfiniteGrid();
	void Render(Algebra::Matrix4 viewMatrix, Algebra::Matrix4 projectionMatrix, Algebra::Vector4 cameraPosition);
};

